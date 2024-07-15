/*
MIT License

Copyright (c) 2021-2024 Magnus

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
#ifndef SRC_CONFIG_HPP_
#define SRC_CONFIG_HPP_

#include <baseconfig.hpp>
#include <utils.hpp>

class GravmonGatewayConfig : public BaseConfig {
 private:
  int _configVersion = 2;

  String _token = "";
  char _gravityFormat = 'G';
  String _timezone = "";
  bool _bleActiveScan = false;
  int _bleScanTime = 5;
  int _pushResendTime = 300;

  // Other
  bool _darkMode = false;

  void formatFileSystem();

 public:
  GravmonGatewayConfig(String baseMDNS, String fileName);
  int getConfigVersion() { return _configVersion; }

  // Token parameter
  const char* getToken() { return _token.c_str(); }
  void setToken(String s) {
    _token = s;
    _saveNeeded = true;
  }

  const char* getTimezone() { return _timezone.c_str(); }
  void setTimezone(String s) {
    _timezone = s;
    _saveNeeded = true;
  }

  int getBleScanTime() { return _bleScanTime; }
  void setBleScanTime(int v) {
    _bleScanTime = v;
    _saveNeeded = true;
  }

  int getPushResendTime() { return _pushResendTime; }
  void setPushResendTime(int t) {
    _pushResendTime = t;
    _saveNeeded = true;
  }

  bool getBleActiveScan() { return _bleActiveScan; }
  void setBleActiveScan(bool b) {
    _bleActiveScan = b;
    _saveNeeded = true;
  }

  char getGravityFormat() { return _gravityFormat; }
  void setGravityFormat(char c) {
    if (c == 'G' || c == 'P') {
      _gravityFormat = c;
      _saveNeeded = true;
    }
  }
  bool isGravitySG() { return _gravityFormat == 'G'; }
  bool isGravityPlato() { return _gravityFormat == 'P'; }

  bool isWifiPushActive() {
    return (hasTargetHttpPost() || hasTargetHttpPost2() || hasTargetHttpGet() ||
            hasTargetInfluxDb2() || hasTargetMqtt())
               ? true
               : false;
  }

  // IO functions
  void createJson(JsonObject& doc);
  void parseJson(JsonObject& doc);
};

extern GravmonGatewayConfig myConfig;

#endif  // SRC_CONFIG_HPP_

// EOF
