/*
MIT License

Copyright (c) 2024-2025 Magnus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#include <ble_gateway.hpp>
#include <log.hpp>

SerialDebug mySerial;

void setup() {
  // Main startup
  Log.notice(F("Main: Started setup for." CR));

  int scanTime = 5;
  bool activeScan = true;

  bleScanner.setScanTime(scanTime);
  bleScanner.setAllowActiveScan(activeScan);
  bleScanner.init();
}

void loop() {

  int resendTime = 30;

  bleScanner.scan();

  for (int i = 0; i < NO_TILT_COLORS; i++) {
    TiltData td = bleScanner.getTiltData((TiltColor)i);

    if (td.updated && (td.getPushAge() > resendTime)) {
      Log.notice(F("Main: Type=%s, Gravity=%F, Temp=%F F." CR),
                 bleScanner.getTiltColorAsString((TiltColor)i), td.gravity,
                 td.tempF);

    }
  }

  // Process gravitymon from BLE
  for (int i = 0; i < NO_GRAVITYMON; i++) {
    GravitymonData& gmd = bleScanner.getGravitymonData(i);

    if (gmd.updated && (gmd.getPushAge() > resendTime)) {

      Log.notice(F("Main: Type=%s, Angle=%F Gravity=%F, Temp=%F, Battery=%F, "
                   "Id=%s." CR),
                 gmd.type.c_str(), gmd.angle, gmd.gravity, gmd.tempC,
                 gmd.battery, gmd.id.c_str());
      gmd.setPushed();
    }
  }
}

// EOF
