/*
    _   ___ ___  _   _ ___ _  _  ___  _    ___   ___
   /_\ | _ \   \| | | |_ _| \| |/ _ \| |  / _ \ / __|
  / _ \|   / |) | |_| || || .` | (_) | |_| (_) | (_ |
 /_/ \_\_|_\___/ \___/|___|_|\_|\___/|____\___/ \___|

Log library for Arduino
version 1.1.1
Copyright (c) 2018 thijse
https://github.com/thijse/Arduino-Log

Licensed under the MIT License <http://opensource.org/licenses/MIT>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

20240827 Included into espframework, fixed lint issues and changed names of
LOG_LEVEL_ to avoid collision with other libraries
*/

#include <ArduinoLog.hpp>

void Logging::begin(int level, Print *logOutput, bool showLevel) {
#ifndef ESPFWK_DISABLE_LOGGING
  setLevel(level);
  setShowLevel(showLevel);
  _logOutput = logOutput;
#endif
}

void Logging::setLevel(int level) {
#ifndef ESPFWK_DISABLE_LOGGING
  _level = constrain(level, ESPFWK_LEVEL_SILENT, ESPFWK_LEVEL_VERBOSE);
#endif
}

int Logging::getLevel() const {
#ifndef ESPFWK_DISABLE_LOGGING
  return _level;
#else
  return 0;
#endif
}

void Logging::setShowLevel(bool showLevel) {
#ifndef ESPFWK_DISABLE_LOGGING
  _showLevel = showLevel;
#endif
}

bool Logging::getShowLevel() const {
#ifndef ESPFWK_DISABLE_LOGGING
  return _showLevel;
#else
  return false;
#endif
}

void Logging::setPrefix(printfunction f) {
#ifndef ESPFWK_DISABLE_LOGGING
  _prefix = f;
#endif
}

void Logging::clearPrefix() {
#ifndef ESPFWK_DISABLE_LOGGING
  _prefix = nullptr;
#endif
}

void Logging::setSuffix(printfunction f) {
#ifndef ESPFWK_DISABLE_LOGGING
  _suffix = f;
#endif
}

void Logging::clearSuffix() {
#ifndef ESPFWK_DISABLE_LOGGING
  _suffix = nullptr;
#endif
}

void Logging::print(const __FlashStringHelper *format, va_list args) {
#ifndef ESPFWK_DISABLE_LOGGING
  PGM_P p = reinterpret_cast<PGM_P>(format);
// This copy is only necessary on some architectures (x86) to change a passed
// array in to a va_list.
#ifdef __x86_64__
  va_list args_copy;
  va_copy(args_copy, args);
#endif
  char c = pgm_read_byte(p++);
  for (; c != 0; c = pgm_read_byte(p++)) {
    if (c == '%') {
      c = pgm_read_byte(p++);
#ifdef __x86_64__
      printFormat(c, &args_copy);
#else
      printFormat(c, &args);
#endif
    } else {
      _logOutput->print(c);
    }
  }
#ifdef __x86_64__
  va_end(args_copy);
#endif
#endif
}

void Logging::print(const char *format, va_list args) {
#ifndef ESPFWK_DISABLE_LOGGING
// This copy is only necessary on some architectures (x86) to change a passed
// array in to a va_list.
#ifdef __x86_64__
  va_list args_copy;
  va_copy(args_copy, args);
#endif
  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
#ifdef __x86_64__
      printFormat(*format, &args_copy);
#else
      printFormat(*format, &args);
#endif
    } else {
      _logOutput->print(*format);
    }
  }
#ifdef __x86_64__
  va_end(args_copy);
#endif
#endif
}

void Logging::printFormat(const char format, va_list *args) {
#ifndef ESPFWK_DISABLE_LOGGING
  if (format == '\0') return;
  if (format == '%') {
    _logOutput->print(format);
  } else if (format == 's') {
    char *s = va_arg(*args, char *);
    _logOutput->print(s);
  } else if (format == 'S') {
    __FlashStringHelper *s = va_arg(*args, __FlashStringHelper *);
    _logOutput->print(s);
  } else if (format == 'd' || format == 'i') {
    _logOutput->print(va_arg(*args, int), DEC);
  } else if (format == 'D' || format == 'F') {
    _logOutput->print(va_arg(*args, double));
  } else if (format == 'x') {
    _logOutput->print(va_arg(*args, int), HEX);
  } else if (format == 'X') {
    _logOutput->print("0x");
    // _logOutput->print(va_arg(*args, int), HEX);
    uint16_t h = (uint16_t)va_arg(*args, int);
    if (h < 0xFFF) _logOutput->print('0');
    if (h < 0xFF) _logOutput->print('0');
    if (h < 0xF) _logOutput->print('0');
    _logOutput->print(h, HEX);
  } else if (format == 'p') {
    Printable *obj = reinterpret_cast<Printable *>(va_arg(*args, int));
    _logOutput->print(*obj);
  } else if (format == 'b') {
    _logOutput->print(va_arg(*args, int), BIN);
  } else if (format == 'B') {
    _logOutput->print("0b");
    _logOutput->print(va_arg(*args, int), BIN);
  } else if (format == 'l') {
    _logOutput->print(va_arg(*args, int64_t), DEC);
  } else if (format == 'u') {
    _logOutput->print(va_arg(*args, uint64_t), DEC);
  } else if (format == 'c') {
    _logOutput->print(static_cast<char>(va_arg(*args, int)));
  } else if (format == 'C') {
    char c = static_cast<char>(va_arg(*args, int));
    if (c >= 0x20 && c < 0x7F) {
      _logOutput->print(c);
    } else {
      _logOutput->print("0x");
      if (c < 0xF) _logOutput->print('0');
      _logOutput->print(c, HEX);
    }
  } else if (format == 't') {
    if (va_arg(*args, int) == 1) {
      _logOutput->print("T");
    } else {
      _logOutput->print("F");
    }
  } else if (format == 'T') {
    if (va_arg(*args, int) == 1) {
      _logOutput->print(F("true"));
    } else {
      _logOutput->print(F("false"));
    }
  }
#endif
}

Logging Log = Logging();

// EOF
