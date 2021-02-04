#ifndef __UPDATER_OTA_H
#define __UPDATER_OTA_H
#define ARDUINO_SIGNING 1


const char HTTP_HEADER[] PROGMEM = "<!DOCTYPE html><html lang=\"en\" style=\"height:100%;\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\" /><title>{title}</title><link rel=\"stylesheet\" href=\"{custom-css}\"><script>function goBack() { window.history.back(); }</script></head><body><div class=\"group\" style=\"margin-top:50px;\"><div id=\"header\" style=\"margin-left:40px;\"><h3 style=\"text-align: center; color: #9e9e9e\">{banner}</h3><p style=\"margin-bottom:-3px; font-size:10px;\">{build}</p><p style=\"margin-bottom:-3px; font-size:10px;\">{branch}</p><p style=\"margin-bottom:-3px; font-size:10px;\">{deviceInfo}</p></div><div id=\"body\">";
const char HTTP_HEADERRELOAD[] PROGMEM = "<!DOCTYPE html><html lang=\"en\" style=\"height:100%;\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\" /><meta http-equiv=\"refresh\" content=\"{delay};url={url}\" /><title>{title}</title><link rel=\"stylesheet\" href=\"{custom-css}\"><script>function goBack() { window.history.back(); }</script></head><body><div class=\"group\" style=\"margin-top:50px;\"><div id=\"header\" style=\"margin-left:40px;\"><h3 style=\"text-align: center; color: #9e9e9e\">{banner}</h3><p style=\"margin-bottom:-3px; font-size:10px;\">{build}</p><p style=\"margin-bottom:-3px; font-size:10px;\">{branch}</p><p>{deviceInfo}</p></div><div id=\"body\">";
const char HTTP_FOOTER[] PROGMEM = "</div><div class=\"divStyle\" style=\"text-align: center; margin-top:15px;\">{footer}</div></div></body></html>";
const char HTTP_UPDATER_INDEX[] PROGMEM = "<form style=\"margin-top: 20px;\"method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><button type='submit'>Update</button></form>";
const char HTTP_UPDATER_SUCCESS[] PROGMEM = "<div style=\"text-align: center; color: #43a047\"><h2>Update Success!</h2></div><div style=\"text-align: center; color: #9e9e9e\"><h2>Rebooting...</h2></div>";
const char HTTP_UPDATER_FAILED[] PROGMEM = "<div style=\"text-align: center; color: #e53935\"><h2>Update failed !</h2></div><div style=\"text-align: center; color: #9e9e9e\"><h2>Rebooting...</h2></div>";
const char HTTP_CSS_STYLE[] PROGMEM = "*{box-sizing:border-box}body{background:#efebe9;font-family:-apple-system,BlinkMacSystemFont,\"Segoe UI\",Roboto,Oxygen,Ubuntu,Cantarell,\"Fira Sans\",\"Droid Sans\",\"Helvetica Neue\",sans-serif;color:#263238;padding:0 auto}h4{margin-bottom:-40px;color:#9e9e9e}h3{font-size:18px;margin-top:20px;margin-bottom:5px}form{padding:auto;margin-top:80px}form input{display:block;width:100%;font-size:15px;padding:5px 10px;margin:10px 0;border-radius:5px;border:1px solid #777}form input.error{border-color:red}form button{display:block;width:100%;appearance:none;border-radius:5px;background-color:#cc6e2f;color:#fff;border:none;font-size:15px;height:30px;margin-top:15px}form button:hover{background-color:#9b4810}form button:disabled{background-color:#dbf99f;color:#333}.group{border-style:solid;padding:10px;padding-bottom:20px;border-radius:5px;border-color:#6b695d;margin-bottom:20px}";

const char pubkey[] PROGMEM = R"EOF(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwbxKKiTEFtY1rQQgmUno
Dxb3WCyTkqAV91n7mrj1K45RB2unKhhoJnHPAfTIwkyJgWTSJf5Ik1QWgz1wGjiG
EFV+84vNM9wP6eqzqIcSaAR7l5UkBxGg8zZIzAJ3JSDNs18NlidLiR1PAgJYGkxb
CDuQgKCTQkD9juQr94HaxeglPsqTty4tyNWp2acXsi9KIgVOcr4ISkb6S+fjZ2bU
l8Y2PjOEHBV3LSRN6TaqwmNNiPVUdwVKKf0o1VbspMLnljPEw3wAKfNUld6+rG6P
AR29BNsKwUqik/c5+P9WabCNE7thj2mDymiyjcNmt3po9v5iVoSNvKZSJCtgCdf1
2QIDAQAB
-----END PUBLIC KEY-----
)EOF";

BearSSL::PublicKey *signPubKey = nullptr;
BearSSL::HashSHA256 *hash;
BearSSL::SigningVerifier *sign;

ESP8266WebServer ota_server(7000);
boolean isAuthenticated = true;

void ICACHE_FLASH_ATTR ota_init() {
  ota_server.on("/style.css",HTTP_GET, []() {
    ota_server.sendHeader("Cache-Control", "public, max-age=31536000"); // useful for adding headers
    ota_server.send(200, "text/css", FPSTR(HTTP_CSS_STYLE));
    });
  ota_server.on("/", HTTP_GET, []() {
    #ifdef enableAuth
    if (!ota_server.authenticate(config_settings.dashUser, config_settings.dashPassword)) {
      return ota_server.requestAuthentication();
    }
    #endif
    String pageIndex = FPSTR(HTTP_HEADER);
    pageIndex.concat(FPSTR(HTTP_UPDATER_INDEX));
    pageIndex.concat(FPSTR(HTTP_FOOTER));
    pageIndex.replace("{title}", _title);
    pageIndex.replace("{banner}", _banner);
    pageIndex.replace("{build}", _build);
    pageIndex.replace("{branch}", _branch);
    pageIndex.replace("{deviceInfo}", _deviceInfo);
    pageIndex.replace("{footer}", _footer);
    pageIndex.replace("{custom-css}", "http://" + _deviceIp + ":" + _devicePort + _css);

    //ota_server.sendHeader("Connection", "close");
    ota_server.send(200, "text/html", pageIndex);
  }); 
  ota_server.on("/update", HTTP_POST, []() {
    #ifdef enableAuth
      if (!ota_server.authenticate(config_settings.dashUser, config_settings.dashPassword)) {
        return ota_server.requestAuthentication();
      }
    #endif
    if (Update.hasError()) {
      String pageFailed = FPSTR(HTTP_HEADERRELOAD);
      pageFailed.concat(FPSTR(HTTP_UPDATER_FAILED));
      pageFailed.concat(FPSTR(HTTP_FOOTER));
      pageFailed.replace("{title}", _title);
      pageFailed.replace("{banner}", _banner);
      pageFailed.replace("{build}", _build);
      pageFailed.replace("{branch}", _branch);
      pageFailed.replace("{deviceInfo}", _deviceInfo);
      pageFailed.replace("{footer}", _footer);
      pageFailed.replace("{url}", "/");
      pageFailed.replace("{delay}", "30");
      pageFailed.replace("{custom-css}", "http://" + _deviceIp + ":" + _devicePort + _css);
      //ota_server.sendHeader("Connection", "close");
      ota_server.send(200, "text/html", pageFailed);
      delay(3000);
      ESP.restart();
    } else {
      String pageSuccess = FPSTR(HTTP_HEADERRELOAD);
      pageSuccess.concat(FPSTR(HTTP_UPDATER_SUCCESS));
      pageSuccess.concat(FPSTR(HTTP_FOOTER));
      pageSuccess.replace("{title}", _title);
      pageSuccess.replace("{banner}", _banner);
      pageSuccess.replace("{build}", _build);
      pageSuccess.replace("{branch}", _branch);
      pageSuccess.replace("{deviceInfo}", _deviceInfo);
      pageSuccess.replace("{footer}", _footer);
      pageSuccess.replace("{url}", "/");
      pageSuccess.replace("{delay}", "30");
      pageSuccess.replace("{custom-css}", "http://" + _deviceIp + ":" + _devicePort + _css);
      //ota_server.sendHeader("Connection", "close");
      ota_server.send(200, "text/html", pageSuccess);
      delay(3000);
      ESP.restart();
    }
  }, []() {
    HTTPUpload& upload = ota_server.upload();
     if (upload.status == UPLOAD_FILE_START) {
    //  Udp.stop();      
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        signPubKey = new BearSSL::PublicKey(pubkey);
        hash = new BearSSL::HashSHA256();
        sign = new BearSSL::SigningVerifier(signPubKey);
        Update.installSignature(hash, sign);       
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (isAuthenticated && upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (isAuthenticated && upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    } else if (isAuthenticated && upload.status == UPLOAD_FILE_ABORTED) {
      Update.end();
      Serial.println(F("#__ Update was aborted\n"));
    }
    yield();
  });
  ota_server.begin();
}

void ota_loop() {
  ota_server.handleClient();
  ota_server.client().setNoDelay(true);
}

#endif
