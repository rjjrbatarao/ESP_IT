//source: https://shepherdingelectrons.blogspot.com/2019/04/esp8266-as-SPIFFS.-http-server.html

#ifndef __WEB_SERVER_H_
#define __WEB_SERVER_H_


//DNSServer dns_server;
ESP8266WebServer web_server(WEB_PORT);
File fsUploadFile;
#include <FS.h>
#include <LittleFS.h>

FS* filesystem = &LittleFS;

void returnOK() {
  web_server.send(200, "text/plain", "");
}

void handleFileList() {
  if (!web_server.hasArg("dir")) {
    web_server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = web_server.arg("dir");
  if (DEBUG_DEV) {
    Serial.println("[HTTP] handleFileList: " + path);
  }
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") {
      output += ',';
    }
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    if (entry.name()[0] == '/') {
      output += &(entry.name()[1]);
    } else {
      output += entry.name();
    }
    output += "\"}";
    entry.close();
  }

  output += "]";
  web_server.send(200, "text/json", output);
}

void handleFileUpload() {
  if (web_server.uri() != "/edit") {
    return;
  }
  HTTPUpload& upload = web_server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    if (DEBUG_DEV) {
      Serial.print(F("[HTTP] handleFileUpload Name: ")); 
      Serial.println(filename);
    }
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (DEBUG_DEV) {
      Serial.print(F("[HTTP] handleFileUpload Data: ")); 
      Serial.println(upload.currentSize);
    }
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    if (DEBUG_DEV) {
      Serial.print(F("[HTTP] handleFileUpload Size: ")); 
      Serial.println(upload.totalSize);
    }
  }
}

void handleFileDelete() {
  if (web_server.args() == 0) {
    return web_server.send(500, "text/plain", "BAD ARGS");
  }
  String path = web_server.arg(0);
  if (DEBUG_DEV) {
    Serial.println("[HTTP] handleFileDelete: " + path);
  }
  if (path == "/") {
    return web_server.send(500, "text/plain", "BAD PATH");
  }
  if (!SPIFFS.exists(path)) {
    return web_server.send(404, "text/plain", "FileNotFound");
  }
  SPIFFS.remove(path);
  web_server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (web_server.args() == 0) {
    return web_server.send(500, "text/plain", "BAD ARGS");
  }
  String path = web_server.arg(0);
  if (DEBUG_DEV) {
    Serial.println("[HTTP] handleFileCreate: " + path);
  }
  if (path == "/") {
    return web_server.send(500, "text/plain", "BAD PATH");
  }
  if (SPIFFS.exists(path)) {
    return web_server.send(500, "text/plain", "FILE EXISTS");
  }
  File file = SPIFFS.open(path, "w");
  if (file) {
    file.close();
  } else {
    return web_server.send(500, "text/plain", "CREATE FAILED");
  }
  web_server.send(200, "text/plain", "");
  path = String();
}

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".xml")) return "image/xml";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  if (DEBUG_DEV) {
    Serial.println("[HTTP] handleFileRead: " + path);
  }
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz)) {
      path += ".gz";
    }
    File file = SPIFFS.open(path, "r");
    web_server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;                                      // If the file doesn't exist, return false
}

void web_loop() {
  //dns_server.processNextRequest();
  web_server.handleClient();
  web_server.client().setNoDelay(true);
}

void ICACHE_FLASH_ATTR web_init() {
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      if (DEBUG_DEV) {
        Serial.printf("[INIT] FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }
    }
    if (DEBUG_DEV) {
      Serial.printf("\n");
    }
  }
  web_server.on("/list", HTTP_GET, handleFileList);
  web_server.on("/edit", HTTP_GET, []() {
    if (!web_server.authenticate(www_username, www_password)) {
      return web_server.requestAuthentication();
    }
    if (!handleFileRead("/edit.htm")) {
      web_server.send(404, "text/plain", "FileNotFound");
    }
  });
  //create file
  web_server.on("/edit", HTTP_PUT, []() {
    if (!web_server.authenticate(www_username, www_password)) {
      return web_server.requestAuthentication();
    }
  }, handleFileCreate);
  //delete file
  web_server.on("/edit", HTTP_DELETE, []() {
    if (!web_server.authenticate(www_username, www_password)) {
      return web_server.requestAuthentication();
    }
  }, handleFileDelete);
  web_server.on("/edit", HTTP_POST, []() {
    if (!web_server.authenticate(www_username, www_password)) {
      return web_server.requestAuthentication();
    }
    returnOK();
  }, handleFileUpload);
  web_server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(web_server.uri()))                  // send it if it exists
      web_server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  web_server.begin(WEB_PORT);
  //web_server.begin();
}


#endif
