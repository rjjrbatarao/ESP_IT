#ifndef __REST_API_H
#define __REST_API_H

// http://10.0.0.203/click?token=10.0.0.214;data=asdasdasd
// http://10.0.0.203/click?ip=10.0.0.214;data=asdasdasd

//using IP will verify from client set IP in mikrotik
//note: this is fixed and cannot be implemented dynamically since ip is cached on router
//using TOKEN will verify from server set token, it is from response key at initial registration in client side
//note: this has advantage when api ssl is enabled, you can create dynamic token on every initial registration
//note: using mikrotik api to change the identity to token and get the value using $(identity)

ESP8266WebServer auto_server(LISTEN_PORT);
const size_t response_capacity = JSON_OBJECT_SIZE(15);
const size_t serial_capacity = JSON_OBJECT_SIZE(5);

void handleRoot() {
  auto_server.send(404, "text/plain", "Not Found\n\n");
}

void handleNotFound() {
  auto_server.send(404, "text/plain", "Not Found\n\n");
}

void handleResponse() {
  
  DynamicJsonDocument json(response_capacity);
  StaticJsonDocument<256> ser;
  char json_buffer[500];
  char ser_buffer[256];
  boolean client_validate = false;
  if (!RESPONSE_DIMENSION_FLAG) {
    //Serial.println("test1");
    for (int i = 0; i < response_count ; i++) {
      if (dataType[i].variableType) {
        json[response[i].data] = response[i].words;
      } else {
        json[response[i].data] = response[i].value;
      }
    }
  } else {
    //Serial.println("test2");
    for (int i; i < event_count; i++) {
      if (strcmp(parameter[i].data, auto_server.uri().c_str()) == 0) {
        int dimension_index = i;
        for (int d_index = 0; d_index < responseDimension[i].dimension_count ; d_index++) {
          if (responseDimension[dimension_index].type[d_index]) {
            json[ responseDimension[dimension_index].data[d_index]] = responseDimension[dimension_index].words[d_index];
          } else {
            json[ responseDimension[dimension_index].data[d_index]] = responseDimension[dimension_index].value[d_index];
          }
        }
      }
    }
  }
  serializeJson(json, json_buffer);
  auto_server.sendHeader("Access-Control-Allow-Origin", "*");
  auto_server.sendHeader("Access-Control-Allow-Methods", "POST"); //"GET, HEAD, POST, PUT, PATCH, DELETE, OPTIONS"
  auto_server.sendHeader("Cache-Control", "no-cache");

  ser["ip"] = auto_server.client().remoteIP().toString();
  for (uint8_t i = 0; i < auto_server.args(); i++) {
    ser[auto_server.argName(i)] = auto_server.arg(i);
  }
  serializeJson(ser, ser_buffer);
  for (int i; i < event_count; i++) {
    if (strcmp(parameter[i].data, auto_server.uri().c_str()) == 0) {
      if (verifySource[i].sourceIp == true && verifySource[i].sourceToken == false) {
        if (strcmp(auto_server.client().remoteIP().toString().c_str(), auto_server.arg(0).c_str()) == 0) {
          auto_server.send (200, "application/json", json_buffer);
          Serial.print(auto_server.uri().substring(1) + ":");
          Serial.println(ser_buffer);
          client_validate = true;
          
        } else {
          auto_server.send(404, "text/plain", "Not Found\n\n");
          client_validate = false;
        }
      } else if (verifySource[i].sourceIp == false && verifySource[i].sourceToken == true) {
        if (strcmp(serverToken, auto_server.arg(0).c_str()) == 0) {
          auto_server.send (200, "application/json", json_buffer);
          Serial.print(auto_server.uri().substring(1) + ":");
          Serial.println(ser_buffer);
          client_validate = true;
          
        } else {
          auto_server.send(404, "text/plain", "Not Found\n\n");
          client_validate = false;
        }
      } else if (verifySource[i].sourceIp == true && verifySource[i].sourceToken == true) {
        auto_server.send(404, "text/plain", "Not Found\n\n");
        client_validate = false;
      } else {
        auto_server.send (200, "application/json", json_buffer);
        Serial.print(auto_server.uri().substring(1) + ":");
        Serial.println(ser_buffer);
        client_validate = true;
        
      }
    } else {
      
      client_validate = false;
    }
  }
  //if(client_validate){
    Serial.println(F("DONE"));
 // }
 
}


void rest_init() {
  // Init variables and expose them to REST API
  if (MACHINE_MODE == AUTO_LOGIN) {
    auto_server.on("/", handleRoot);
    auto_server.on(parameter[0].data, HTTP_POST, handleResponse);
    auto_server.on(parameter[1].data, HTTP_POST, handleResponse);
    auto_server.on(parameter[2].data, HTTP_POST, handleResponse);
    auto_server.on(parameter[3].data, HTTP_POST, handleResponse);
    auto_server.on(parameter[4].data, HTTP_POST, handleResponse);
    auto_server.onNotFound(handleNotFound);
    auto_server.begin(LISTEN_PORT);
    
  }
}

void rest_loop() {
  if (MACHINE_MODE == AUTO_LOGIN) {
    auto_server.handleClient();
    auto_server.client().setNoDelay(true);
  }
}


#endif
