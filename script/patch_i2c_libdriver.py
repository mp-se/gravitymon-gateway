"""
Pre-build script: patch libdriver.a to neutralise the check_i2c_driver_conflict
global constructor inside i2c.c.obj.

Background
----------
ESP-IDF 5.4+ ships two I2C drivers:
  old: driver/i2c.h     (i2c_driver_install, i2c_master_write_to_device …)
  new: driver/i2c_master.h (i2c_new_master_bus …) — used by Arduino Wire on IDF >= 5.4

When both object files are linked the old driver's check_i2c_driver_conflict()
— registered via __attribute__((constructor)) / .ctors — detects that
i2c_acquire_bus_handle (from the new driver) is non-NULL and calls abort(),
crashing before setup() runs.

The ESP32_IO_Expander library calls i2c_master_write_to_device (old API) for
CH422G I/O expander communication, so i2c.c.obj from libdriver.a MUST be linked.

Fix: zero out the .ctors section header size in i2c.c.obj so the linker includes
the object file's I2C functions but does NOT register check_i2c_driver_conflict
as a global constructor.  We operate on a LOCAL copy so the system package is
never modified.  The linker is redirected to the patched copy via LIBPATH.
"""

import os
import shutil
import struct
import subprocess

Import("env")  # noqa: F821 — injected by PlatformIO

# Only needed for the Waveshare 4.3" RGB panel environment
defines = [d[0] if isinstance(d, tuple) else d for d in env.get("CPPDEFINES", [])]
if "WAVESHARE_S3_TFT43" not in defines:
    Return()  # noqa: F821 — SCons Return()


def _find_pkg(pkg_name_prefix):
    pio_home = os.path.expanduser("~/.platformio")
    pkg_dir = os.path.join(pio_home, "packages")
    if not os.path.isdir(pkg_dir):
        raise FileNotFoundError(f"PlatformIO packages dir not found: {pkg_dir}")
    exact = os.path.join(pkg_dir, pkg_name_prefix)
    if os.path.isdir(exact):
        return exact
    for name in sorted(os.listdir(pkg_dir)):
        if name.startswith(pkg_name_prefix):
            return os.path.join(pkg_dir, name)
    raise FileNotFoundError(
        f"PlatformIO package '{pkg_name_prefix}' not found in {pkg_dir}"
    )


AR = os.path.join(_find_pkg("toolchain-xtensa-esp-elf"), "bin", "xtensa-esp-elf-ar")
SRC_LIB = os.path.join(
    _find_pkg("framework-arduinoespressif32-libs"), "esp32s3", "lib", "libdriver.a"
)
PATCH_DIR = os.path.join(
    env.subst("$PROJECT_BUILD_DIR"), env.subst("$PIOENV"), "patched_libs"
)
DST_LIB = os.path.join(PATCH_DIR, "libdriver.a")
OBJ_NAME = "i2c.c.obj"


def _zero_elf_ctors(obj_path):
    """
    Zero out the .ctors section size in an ELF32-LE object file so the contained
    __attribute__((constructor)) is never registered as a global constructor.
    The function body is left intact (it is still reachable if called explicitly,
    but the runtime never invokes it).
    """
    with open(obj_path, "r+b") as f:
        data = bytearray(f.read())

    if data[:4] != b"\x7fELF":
        raise ValueError(f"Not an ELF file: {obj_path}")

    e_shoff = struct.unpack_from("<I", data, 0x20)[0]
    e_shentsize = struct.unpack_from("<H", data, 0x2E)[0]
    e_shnum = struct.unpack_from("<H", data, 0x30)[0]
    e_shstrndx = struct.unpack_from("<H", data, 0x32)[0]

    strtab_hdr = e_shoff + e_shstrndx * e_shentsize
    strtab_off = struct.unpack_from("<I", data, strtab_hdr + 0x10)[0]
    strtab_sz = struct.unpack_from("<I", data, strtab_hdr + 0x14)[0]
    strtab = data[strtab_off : strtab_off + strtab_sz]

    def name_of(idx):
        ni = struct.unpack_from("<I", data, e_shoff + idx * e_shentsize)[0]
        end = strtab.index(b"\x00", ni)
        return strtab[ni:end].decode()

    ctors_idx = None
    for i in range(e_shnum):
        if name_of(i) == ".ctors":
            ctors_idx = i
            break

    if ctors_idx is None:
        return  # Nothing to patch

    patched = False
    for i in range(e_shnum):
        n = name_of(i)
        sh_base = e_shoff + i * e_shentsize
        sh_type = struct.unpack_from("<I", data, sh_base + 4)[0]
        sh_info = struct.unpack_from("<I", data, sh_base + 0x1C)[0]
        # Zero the size of .ctors and of its relocation section
        if i == ctors_idx or (sh_type in (4, 9) and sh_info == ctors_idx):
            struct.pack_into("<I", data, sh_base + 0x14, 0)
            patched = True

    if patched:
        with open(obj_path, "wb") as f:
            f.write(data)


def patch_libdriver():
    os.makedirs(PATCH_DIR, exist_ok=True)

    if os.path.exists(DST_LIB) and (
        os.path.getmtime(DST_LIB) >= os.path.getmtime(SRC_LIB)
    ):
        print("patch_i2c_libdriver: patched libdriver.a is up-to-date")
        return

    print("patch_i2c_libdriver: patching libdriver.a — removing i2c constructor …")
    shutil.copy2(SRC_LIB, DST_LIB)

    # Extract i2c.c.obj from the copy
    obj_path = os.path.join(PATCH_DIR, OBJ_NAME)
    subprocess.check_call([AR, "-x", DST_LIB, OBJ_NAME], cwd=PATCH_DIR)

    # Patch: zero out .ctors section so check_i2c_driver_conflict is not a constructor
    _zero_elf_ctors(obj_path)

    # Replace the original entry in the archive with the patched object
    subprocess.check_call([AR, "-r", DST_LIB, obj_path], cwd=PATCH_DIR)

    os.remove(obj_path)
    print("patch_i2c_libdriver: done — i2c constructor neutralised in patched copy")


patch_libdriver()

# Prepend patched-libs dir so the linker finds our copy before the system one
env.Prepend(LIBPATH=[PATCH_DIR])
