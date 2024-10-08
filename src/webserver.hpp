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
#ifndef SRC_WEBSERVER_HPP_
#define SRC_WEBSERVER_HPP_

#include <basewebserver.hpp>
#include <blescanner.hpp>

class GravmonGatewayWebServer : public BaseWebServer {
 private:
  volatile bool _pushTestTask = false;

  String _pushTestTarget;
  int _pushTestLastCode;
  bool _pushTestLastSuccess, _pushTestEnabled;

  GravitymonData _gravitymon[NO_GRAVITYMON];

  void webHandleStatus(AsyncWebServerRequest *request);
  void webHandleConfigRead(AsyncWebServerRequest *request);
  void webHandleConfigWrite(AsyncWebServerRequest *request, JsonVariant &json);
  void webHandleConfigFormatRead(AsyncWebServerRequest *request);
  void webHandleConfigFormatWrite(AsyncWebServerRequest *request,
                                  JsonVariant &json);
  void webHandleTestPush(AsyncWebServerRequest *request, JsonVariant &json);
  void webHandleTestPushStatus(AsyncWebServerRequest *request);
  void webHandleFactoryDefaults(AsyncWebServerRequest *request);
  void webHandleRemotePost(AsyncWebServerRequest *request, JsonVariant &json);

  String readFile(String fname);
  bool writeFile(String fname, String data);

 public:
  explicit GravmonGatewayWebServer(WebConfig *config);

  int findGravitymonId(String id) {
    for (int i = 0; i < NO_GRAVITYMON; i++)
      if (_gravitymon[i].id == id || _gravitymon[i].id == "") return i;
    return -1;
  }
  GravitymonData &getGravitymonData(int idx) { return _gravitymon[idx]; }

  bool setupWebServer();
  void loop();
};

// Global instance created
extern GravmonGatewayWebServer myWebServer;

#endif  // SRC_WEBSERVER_HPP_

// EOF
