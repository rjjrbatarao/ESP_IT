#ifndef __CLI_COM_H
#define __CLI_COM_H
// Create CLI Object
SimpleCLI cli;

// Commands
Command api;
Command config;
Command wifi;
Command connect;
Command rest;
Command server;
Command event;
Command type;
Command license;
Command verify;
Command verifier;
Command dimensions;
Command task;
Command timer;
Command eeprom;
Command web;
Command sse;


IPAddress mikrotik_server;
WiFiServer sse_server(3000);
WiFiClient api_client;
WiFiClientSecure ssl_client;
mikrotikapiwifi mk(api_client);
mikrotikapiwifi mk_ssl(ssl_client);


void ICACHE_FLASH_ATTR eepromCallback(cmd* c) {
  if (LICENSE_STATUS) {
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    boolean eeprom_mode = false;
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      if (i == 0) {
        if (strcmp((char*)arg.getValue().c_str() , "-g") == 0) {
          eeprom_mode = false;
          Serial.printf("eeprom:%s\n", eeprom_get_data(66));
        } else if (strcmp((char*)arg.getValue().c_str() , "-p") == 0) {
          eeprom_mode = true;
        }
      } else if (i == 1) {
        if (eeprom_mode) {
          uint8_t eeprom_len = strlen((char*)arg.getValue().c_str());
          if (eeprom_len <= 256) {
            eeprom_put_data(66, (char*)arg.getValue().c_str());
            Serial.println("test");
          }
        }
      }
      Serial.println(F("DONE"));
    }
  }
}

void ICACHE_FLASH_ATTR timerCallback(cmd * c) {
  if (LICENSE_STATUS) {
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    uint8_t timer_index;
    uint8_t timer_interval;
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      if (i == 0) {
        timer_index = atoi((char*)arg.getValue().c_str());
      } else if (i == 1) {
        if (strcmp((char*)arg.getValue().c_str() , "-o") != 0) {
          strcpy(flexibleTimer[timer_index].name, (char*)arg.getValue().c_str());
        }
      } else if (i == 2) {
        if (strcmp((char*)arg.getValue().c_str() , "-e") == 0) {

        } else if (strcmp((char*)arg.getValue().c_str() , "-d") == 0) {
          timerDisable(timer_index);
        }
      } else if (i == 3) {
        timer_interval = atoi((char*)arg.getValue().c_str());
      } else if (i == 4) {
        uint8_t timer_seconds = atoi((char*)arg.getValue().c_str());
        uint8_t timer_final = (timer_seconds <= 3600 && timer_seconds >= 1) ? timer_seconds : 1;
        scheduleDynamicTask(timer_final, timer_index, timer_interval);
      }
    }
    Serial.println(F("DONE"));
  }
}


void ICACHE_FLASH_ATTR taskCallback(cmd * c) {

  Command cmd(c);               // Create wrapper object
  uint8_t argNum = cmd.countArgs(); // Get number of arguments
  uint8_t task_index;
  uint8_t code_length;
  boolean upper_case = false;
  for (int i = 0; i < argNum; i++) {
    Argument arg = cmd.getArg(i);
    if (i == 0) {
      task_index = atoi((char*)arg.getValue().c_str());
    } else if (i == 1) {
      if (strcmp((char*)arg.getValue().c_str() , "-o") == 0) {

      } else {
        strcpy(taskNames[task_index].name, (char*)arg.getValue().c_str());
      }
    } else if (i == 2) {
      switch (task_index) {
        case 0:
          if (strcmp((char*)arg.getValue().c_str() , "-ss") == 0) {
            wifiGetLinkStatus(taskNames[0].name);
          }
          break;
        case 1:
          if (LICENSE_STATUS && WIFI_IS_CONNECTED) {
            if (strcmp((char*)arg.getValue().c_str() , "-l") == 0) {
              upper_case = false;
            } else if (strcmp((char*)arg.getValue().c_str() , "-u") == 0) {
              upper_case = true;
            }
          }
          break;
        case 2:
          if (strcmp((char*)arg.getValue().c_str() , "-gt") == 0) {
            getBoardInfo();
          }
          break;
      }
    } else if (i == 3) {
      switch (task_index) {
        case 0:

          break;
        case 1:
          if (LICENSE_STATUS && WIFI_IS_CONNECTED) {
            ESP8266TrueRandom.uuid(uuidNumber);
            char lower_case_buffer[25];
            char upper_case_buffer[25];

            if (strcmp((char*)arg.getValue().c_str() , "-gn") == 0) {
              strcpy(lower_case_buffer, ESP8266TrueRandom.uuidToString(uuidNumber).substring(0, 16).c_str());
              if (upper_case) {
                for (int i = 0; i < strlen(lower_case_buffer); i++) {
                  upper_case_buffer[i] = toupper(lower_case_buffer[i]);
                }
                upper_case_buffer[strlen(lower_case_buffer)] = '\0';
                Serial.printf("%s:%s\n", taskNames[1].name, upper_case_buffer);
              } else {
                Serial.printf("%s:%s\n", taskNames[1].name, lower_case_buffer);
              }
            } else {
              code_length = atoi((char*)arg.getValue().c_str());
              uint8_t final_length = (code_length <= 16 && code_length >= 4) ? code_length : 8;
              strcpy(lower_case_buffer, ESP8266TrueRandom.uuidToString(uuidNumber).substring(0, final_length).c_str());
              if (upper_case) {
                for (int i = 0; i < code_length; i++) {
                  upper_case_buffer[i] = toupper(lower_case_buffer[i]);
                }
                upper_case_buffer[code_length] = '\0';
                Serial.printf("%s:%s\n", taskNames[1].name, upper_case_buffer);
              } else {
                Serial.printf("%s:%s\n", taskNames[1].name, lower_case_buffer);
              }
            }
          }
          break;
        case 2:

          break;
      }
    }
  }
  Serial.println(F("DONE"));

}

void ICACHE_FLASH_ATTR dimensionCallback(cmd * c) {
  if (MACHINE_MODE == AUTO_LOGIN && RESPONSE_DIMENSION_FLAG == true && WIFI_IS_CONNECTED) {
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    uint8_t dimension_index;
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      char argValue[64];
      strcpy(argValue, (char*)arg.getValue().c_str());
      if (i == 0) {
        dimension_index = atoi(argValue);
      } else {
        if (arg_counts != i - 1) {
          strcpy(responseDimension[dimension_index].data[responseDimension[dimension_index].dimension_count], argValue);
          //Serial.println(responseDimension[dimension_index].data[responseDimension[dimension_index].dimension_count]);
          responseDimension[dimension_index].dimension_count++;
        }
        if (i == argNum - 1) {
          break;
        }
      }
    }
  }
  Serial.println(F("DONE"));
}

void ICACHE_FLASH_ATTR verifyCallback(cmd * c) {
  if (MACHINE_MODE == AUTO_LOGIN && WIFI_IS_CONNECTED) {
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      verifySource[i].sourceIp = atoi((char*)arg.getValue().c_str());
      //Serial.println(verifySource[i].sourceIp);
      if (i == 4) {
        break;
      }
    }
  }
  Serial.println(F("DONE"));
}

void ICACHE_FLASH_ATTR verifierCallback(cmd * c) {
  if (MACHINE_MODE == AUTO_LOGIN && WIFI_IS_CONNECTED) {
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    uint8_t generate_random = false;
    uint8_t token_length;
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      if (i == 0) {
        if (strcmp((char*)arg.getValue().c_str() , "-o") == 0) {

        } else {
          strcpy(serverToken, (char*)arg.getValue().c_str());
          Serial.printf("token:%s\n", serverToken);
        }
      } else if (i == 1) {
        if (strcmp((char*)arg.getValue().c_str() , "-o") == 0) {

        } else {
          token_length = atoi((char*)arg.getValue().c_str());
          ESP8266TrueRandom.uuid(uuidNumber);
          memset(serverToken, 0, sizeof(serverToken));
          sprintf(serverToken, "%s", ESP8266TrueRandom.uuidToString(uuidNumber).substring(0, (token_length <= 16 && token_length >= 8) ?
                  token_length : TOKEN_LENGTH).c_str());
          Serial.printf("token:%s\n", serverToken);
        }
      } else {
        verifySource[i - 2].sourceToken = atoi((char*)arg.getValue().c_str());
        //Serial.println(verifySource[i - 1].sourceToken);
        if (i == 6) {
          break;
        }
      }
    }
  }
  Serial.println(F("DONE"));
}

void ICACHE_FLASH_ATTR licenseCallback(cmd * c) {
  if (!LICENSE_STATUS) {
    eventFlag = true;
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      if (i == 0) {
        licenseWrite((char*)arg.getValue().c_str());
      }
    }
  }
}


void ICACHE_FLASH_ATTR eventCallback(cmd * c) {
  if (MACHINE_MODE == AUTO_LOGIN && eventFlag == false && WIFI_IS_CONNECTED) {
    eventFlag = true;
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      sprintf(parameter[i].data, "/%s", (char*)arg.getValue().c_str());
      //Serial.println(parameter[i].data);
      event_count++;
      if (i == 4) {
        break;
      }
    }
  }
  Serial.println(F("DONE"));
}

void ICACHE_FLASH_ATTR typeCallback(cmd * c) {
  if (MACHINE_MODE == AUTO_LOGIN && WIFI_IS_CONNECTED) {
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    uint8_t dimension_index;
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      if (!RESPONSE_DIMENSION_FLAG) {
        if (i != arg_counts - 1) {
          dataType[i].variableType = atoi((char*)arg.getValue().c_str());
        }
      } else {
        if (i == 0) {
          dimension_index = atoi((char*)arg.getValue().c_str());
        } else {
          if (i !=  responseDimension[dimension_index].dimension_count + 1) {
            responseDimension[dimension_index].type[i - 1] = atoi((char*)arg.getValue().c_str());
            //Serial.println(responseDimension[dimension_index].type[i - 1]);
          }
        }
      }
    }
  }
}

void ICACHE_FLASH_ATTR restCallback(cmd * c) {
  if (MACHINE_MODE == AUTO_LOGIN && WIFI_IS_CONNECTED) {
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      char argValue[64];
      strcpy(argValue, (char*)arg.getValue().c_str());
      if (!RESPONSE_DIMENSION_FLAG) {
        if (arg_counts != i) {
          if (strcmp(argValue , "-o") != 0) {
            if (dataType[i].variableType) {
              strcpy(response[i].words, argValue);
            } else {
              response[i].value = atoi(argValue);
            }
          }
        } else if (i == response_count) {
          break;
        }
      } else {
        if (i == 0) {
          current_index = atoi(argValue);
        } else {
          if (arg_counts != i - 1) {
            if (strcmp(argValue , "-o") != 0) {
              if (responseDimension[current_index].type[i - 1]) {
                strcpy(responseDimension[current_index].words[i - 1], argValue);
              } else {
                responseDimension[current_index].value[i - 1] = atoi(argValue);
              }
            }
          } else if (i == responseDimension[current_index].dimension_count + 1) {
            break;
          }
        }
      }
    }
  }
  Serial.println(F("DONE"));
}



void ICACHE_FLASH_ATTR serverCallback(cmd * c) {
  if (MACHINE_MODE == AUTO_LOGIN && serverFlag == false && WIFI_IS_CONNECTED) {
    REST_MODE = true;
    serverFlag = true;
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    // Go through all arguments and add their value up
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      char argValue[64];
      strcpy(argValue, (char*)arg.getValue().c_str());
      if (i == 0) {
        LISTEN_PORT = atoi(argValue);
      } else {
        if (!RESPONSE_DIMENSION_FLAG) {
          if (arg_counts != i - 1) {
            if (strcmp(argValue , "-o") != 0) {
              strcpy(response[response_count].data, argValue);
              //Serial.println(response[response_count].data);
            }
            response_count++;
          }
          if (i == argNum - 1) {
            break;
          }
        }
      }
    }
    auto start = millis();
    rest_init();
    if (DEBUG_DEV) {
      Serial.print(F("[TIME] Milliseconds:"));
      auto time = millis() - start;
      Serial.print(F("[TIME] Milliseconds:"));
      Serial.println(time);
      Serial.println(F("[INIT] Server: initialized"));
    }
  }
  Serial.println(F("DONE"));
}

void ICACHE_FLASH_ATTR webCallback(cmd * c) {
  if (webFlag == false) { // soon to be filled with logic
    WEB_MODE = true;
    webFlag = true;
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    // Go through all arguments and add their value up
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      char argValue[64];
      strcpy(argValue, (char*)arg.getValue().c_str());
      if (i == 0) {
        WEB_PORT = atoi(argValue);
      } else {
        if (strcmp(argValue , "-o") != 0) {

        }
      }
    }
    auto start = millis();
    web_init();
    ota_init();
    if (DEBUG_DEV) {
      Serial.print(F("[TIME] Milliseconds:"));
      auto time = millis() - start;
      Serial.print(F("[TIME] Milliseconds:"));
      Serial.println(time);
      Serial.println(F("[INIT] Web: initialized"));
    }
  }
  Serial.println(F("DONE"));
}

void ICACHE_FLASH_ATTR apiCallback(cmd * c) {
  if (WIFI_IS_CONNECTED) {
    Command cmd(c);               // Create wrapper object
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    auto start = millis();
    if (SSL) {
      mk_ssl.apiDisconnect();// this solves wifi memory leak
      mk_ssl.apiConnect(mikrotik_server, mikrotikUsername, mikrotikPassword, port);
    } else {
      mk.apiDisconnect();// this solves wifi memory leak
      mk.apiConnect(mikrotik_server, mikrotikUsername, mikrotikPassword, port);
    }
    // Go through all arguments and add their value up
    for (int i = 0; i < argNum; i++) {
      Argument arg = cmd.getArg(i);
      char argValue[128];
      strcpy(argValue, (char*)arg.getValue().c_str());
      if (SSL) {
        mk_ssl.writeWord(argValue);
      } else {
        mk.writeWord(argValue);
      }
    }
    if (SSL) {
      mk_ssl.writeWord("");
      (mk_ssl.readStreamSentence() == DONE) ? 1 : 0;
    } else {
      mk.writeWord("");
      (mk.readStreamSentence() == DONE) ? 1 : 0;
    }

    if (DEBUG_DEV) {
      auto time = millis() - start;
      Serial.print(F("[TIME] Milliseconds:"));
      Serial.println(time);
    }
    Serial.println("DONE");
  }
}

void ICACHE_FLASH_ATTR configCallback(cmd * c) {
  Command cmd(c);               // Create wrapper object
  uint8_t argNum = cmd.countArgs(); // Get number of arguments
  auto start = millis();
  for (int i = 0; i < argNum; i++) {
    Argument arg = cmd.getArg(i);
    String argValue = arg.getValue();
    if (i == 0) {
      if (strcmp((char*)argValue.c_str() , "-d") == 0) {
        DEBUG_DEV = 1;
      } else if (strcmp((char*)argValue.c_str() , "-o") == 0) {
        DEBUG_DEV = 0;
      } else if (strcmp((char*)argValue.c_str() , "-r") == 0) {
        ESP.restart();
      } else {
        DEBUG_DEV = 0;
      }
    } else if (i == 1) {
      if (strcmp((char*)argValue.c_str() , "-s") == 0) {
        if (LICENSE_STATUS) {
          SSL = true;
        } else {
          SSL = false;
        }
      } else if (strcmp((char*)argValue.c_str() , "-x") == 0) {
        AXTLS = true;
      } else if (strcmp((char*)argValue.c_str() , "-o") == 0) {
        SSL = false;
      } else {

      }
    } else if (i == 2) {
      if (strcmp((char*)argValue.c_str() , "-o") == 0) {

      } else {
        if (LICENSE_STATUS) {
          argValue.replace(":", " ");
          strcpy(fingerprint, (char*)argValue.c_str());
          Serial.println(F("[INIT] Mode: Api-SSL"));
        }
      }
    } else if (i == 3) {
      if (strcmp((char*)argValue.c_str() , "-b") == 0) {
        MACHINE_MODE = BASIC_LOGIN;
        if (DEBUG_DEV) {
          Serial.println(F("[INIT] Type: BASIC"));
        }
      } else if (strcmp((char*)argValue.c_str() , "-a") == 0) {
        if (LICENSE_STATUS) {
          MACHINE_MODE = AUTO_LOGIN;
          strcpy(parameter[0].data, "ev_0");
          strcpy(parameter[1].data, "ev_1");
          strcpy(parameter[2].data, "ev_2");
          strcpy(parameter[3].data, "ev_3");
          strcpy(parameter[4].data, "ev_4");
          strcpy(taskNames[0].name, "link");
          strcpy(taskNames[1].name, "code");
          strcpy(taskNames[2].name, "info");
          //strcpy(taskNames[3].name, "");
          //strcpy(taskNames[1].name[2], "");
          strcpy(flexibleTimer[0].name, "tm_0");
          strcpy(flexibleTimer[1].name, "tm_1");
          strcpy(flexibleTimer[2].name, "tm_2");
          strcpy(flexibleTimer[3].name, "tm_3");
          strcpy(flexibleTimer[4].name, "tm_4");
          event_count = 4;
          if (DEBUG_DEV) {
            Serial.println(F("[INIT] Type: AUTO"));
          }
        }
      }
    } else if (i == 4) {
      if (LICENSE_STATUS) {
        if (strcmp((char*)argValue.c_str() , "-rd") == 0) {
          RESPONSE_DIMENSION_FLAG = true;
          if (DEBUG_DEV) {
            Serial.println(F("[INIT] Dimension: enabled"));
          }
        } else {
          RESPONSE_DIMENSION_FLAG = false;
        }
      }
    } else {

    }
  }
  if (DEBUG_DEV) {
    auto time = millis() - start;
    Serial.print(F("[TIME] Milliseconds:"));
    Serial.println(time);
  }
  Serial.println("DONE");
}


void ICACHE_FLASH_ATTR wifiCallback(cmd * c) {
  if (wifiFlag == false) {
    wifiFlag = true;
    Command cmd(c);               // Create wrapper object
    uint8_t conn_tries = 0;
    uint8_t WIFI_RETRIES = 100;
    uint8_t argNum = cmd.countArgs(); // Get number of arguments
    auto start = millis();
    for (int i = 0; i < argNum; i++) {
      Argument arg      = cmd.getArg(i);
      String argValue = arg.getValue();
      if (i == 0) {
        if (strcmp((char*)argValue.c_str() , "-o") == 0) {
          SMART_CONNECT = true;
        } else {
          strcpy(sta_ssid, (char*)argValue.c_str());
        }
      } else if (i == 1) {
        if (strcmp((char*)argValue.c_str() , "-o") == 0) {
          strcpy(sta_password, "");
        } else {
          strcpy(sta_password, (char*)argValue.c_str());
        }
      } else if (i == 2) {
        if (strcmp((char*)argValue.c_str() , "-o") == 0) {
          strcpy(ap_ssid, "ESP0000");
        } else {
          strcpy(ap_ssid, (char*)argValue.c_str());
          SMART_CONNECT = false;
        }
      } else if (i == 3) {
        if (strcmp((char*)argValue.c_str() , "-o") == 0) {
          strcpy(ap_password, "");
        } else {
          strcpy(ap_password, (char*)argValue.c_str());
        }
      } else if (i == 4) {
        if (strcmp((char*)argValue.c_str() , "-a") == 0) {
          WIFI_MODE = AP_MODE;
        } else if (strcmp((char*)argValue.c_str() , "-s") == 0) {
          WIFI_MODE = STA_MODE;
        } else if (strcmp((char*)argValue.c_str() , "-as") == 0) {
          WIFI_MODE = AP_STA_MODE;
        } else {
          WIFI_MODE = STA_MODE;
        }
      } else if (i == 5) {
        if (strcmp((char*)argValue.c_str() , "-b") == 0) {
          WIFI_PHY_MODE = B_PHY;
        } else if (strcmp((char*)argValue.c_str() , "-g") == 0) {
          WIFI_PHY_MODE = G_PHY;
        } else if (strcmp((char*)argValue.c_str() , "-n") == 0) {
          WIFI_PHY_MODE = N_PHY;
        } else {
          WIFI_PHY_MODE = G_PHY;
        }
      } else if (i == 6) {
        if (strcmp((char*)argValue.c_str() , "-l") == 0) {
          PHY_MIN_FLAG = true;
        } else  if (strcmp((char*)argValue.c_str() , "-h") == 0) {
          PHY_MIN_FLAG = false;
        } else {
          PHY_MIN_FLAG = false;
        }
      } else if (i == 7) {

      }
    }

    switch (WIFI_PHY_MODE) {
      case B_PHY:
        WiFi.setPhyMode(WIFI_PHY_MODE_11B);
        if (PHY_MIN_FLAG) {
          WiFi.setOutputPower(B_DB_MIN);
        } else {
          WiFi.setOutputPower(B_DB_MAX);
        }
        break;
      case G_PHY:
        WiFi.setPhyMode(WIFI_PHY_MODE_11G);
        if (PHY_MIN_FLAG) {
          WiFi.setOutputPower(G_DB_MIN);
        } else {
          WiFi.setOutputPower(G_DB_MAX);
        }
        break;
      case N_PHY:
        WiFi.setPhyMode(WIFI_PHY_MODE_11N);
        if (PHY_MIN_FLAG) {
          WiFi.setOutputPower(N_DB_MIN);
        } else {
          WiFi.setOutputPower(N_DB_MAX);
        }
        break;
    }
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    //WiFi.forceSleepWake();
    IPAddress apIP(192, 168, 1, 1);
    IPAddress netMsk(255, 255, 255, 0);
    if (!SMART_CONNECT) {
      switch (WIFI_MODE) {
        case STA_MODE:
          //WiFi.mode(WIFI_STA);
          //WiFi.begin(sta_ssid, sta_password);
          if (WiFi.SSID() != sta_ssid) {
            Serial.println(F("Initialising Wifi sta"));
            WiFi.mode(WIFI_STA);
            WiFi.begin(sta_ssid, sta_password);
            WiFi.persistent(true);
            WiFi.setAutoConnect(true);
            WiFi.setAutoReconnect(true);
          }
          break;
        case AP_MODE:
          Serial.println(F("Initialising Wifi ap"));
          WiFi.persistent(false);
          WiFi.disconnect(true);
          WiFi.softAPConfig(apIP, apIP, netMsk);
          WiFi.mode(WIFI_AP);
          WiFi.softAP(ap_ssid, ap_password, 1);
          //dns_server.setErrorReplyCode(DNSReplyCode::NoError);
          //dns_server.start(DNS_PORT, "*", apIP);
          break;
        case AP_STA_MODE:
          Serial.println(F("Initialising Wifi ap sta"));
          WiFi.softAPConfig(apIP, apIP, netMsk);
          WiFi.mode(WIFI_AP_STA);
          WiFi.softAP(ap_ssid, ap_password);
          WiFi.begin(sta_ssid, sta_password);

          break;
      }
      if (WIFI_MODE != AP_MODE) {
        wifi_station_connect();// test if deep sleep problem solved
        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
          Serial.println(F("Connection Failed!"));
        }
      }
    } else {
      if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println(F("Connection Failed!"));
        WiFi.mode(WIFI_STA);
        /* start SmartConfig */
        WiFi.beginSmartConfig();
        WiFi.persistent(true);
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
        /* Wait for SmartConfig packet from mobile */
        Serial.println(F("Waiting for SmartConfig."));
        while (!WiFi.smartConfigDone()) {
          delay(500);
          Serial.println(F("init:wait"));
        }
        Serial.println(F("[INIT] SmartConfig: done."));
        WIFI_IS_CONNECTED = true;
      } else {

      }
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println(F("init:wait"));
      }
    }
    
    if (wifiGetLinkStatus("wifi") != 0) {
      WIFI_IS_CONNECTED = true;
      mikrotik_server = WiFi.gatewayIP();
      if (DEBUG_DEV) {
        Serial.println(F(""));
        switch (WIFI_MODE) {
          case STA_MODE:
            Serial.println(F("[INIT] WiFi: connected"));
            Serial.print(F("[INIT] IP address: "));
            Serial.println(WiFi.localIP());
            Serial.print(F("[INIT] Gateway: "));
            Serial.println(mikrotik_server);
            break;
          case AP_MODE:
            Serial.println(F("[INIT] WiFi: initialized"));
            Serial.print(F("[INIT] IP address: "));
            Serial.println(WiFi.softAPIP());
            break;
          case AP_STA_MODE:
            Serial.println(F("[INIT] WiFi: initialized"));
            break;
        }
        Serial.print(F("[INIT] Mac: "));
        Serial.println(WiFi.macAddress());
        Serial.print(F("[INIT] Rssi: "));
        Serial.println(WiFi.RSSI());
        auto time = millis() - start;
        Serial.print(F("[TIME] Milliseconds:"));
        Serial.println(time);
      }

      if (SSL) {
        ssl_client.setBufferSizes(768, 256);
        ssl_client.setFingerprint(fingerprint);
      }
      Serial.println(F("DONE"));
    } else {
      Serial.println(F("FAIL"));
    }
    //Serial.println("[INIT] Wifi Status: connected");
  } else {
    Serial.println(F("DONE"));
  }
}

void ICACHE_FLASH_ATTR connectCallback(cmd * c) {
  Command cmd(c);               // Create wrapper object
  uint8_t argNum = cmd.countArgs(); // Get number of arguments
  auto start = millis();
  for (int i = 0; i < argNum; i++) {
    Argument arg  = cmd.getArg(i);
    String argValue = arg.getValue();
    //Serial.println(argValue);
    if (i == 0) {
      strcpy(mikrotikUsername, (char*)argValue.c_str());
    } else if (i == 1) {
      if (strcmp((char*)argValue.c_str() , "-o") == 0) {
        strcpy(mikrotikPassword, "");
      } else {
        strcpy(mikrotikPassword, (char*)argValue.c_str());
      }
    } else if (i == 2) {
      port = atoi((char*)argValue.c_str());
    } else {

    }
  }
  Serial.print(F("[INIT] Connecting to: "));
  Serial.println(mikrotik_server);
  Serial.println("");
  boolean status = false;
  if (SSL) {
    status = mk_ssl.apiConnect(mikrotik_server, mikrotikUsername, mikrotikPassword, port);
  } else {
    status = mk.apiConnect(mikrotik_server, mikrotikUsername, mikrotikPassword, port);
  }

  if (status) { // login user
    if (DEBUG_DEV) {
      Serial.println(F("[INIT] Connection: success"));
      Serial.print(F("[INIT] On Port: "));
      Serial.println(port);
      auto time = millis() - start;
      Serial.print(F("[INIT] TIME: "));
      Serial.println(time);
      Serial.println(F("[INIT] Connection: persistent"));
    }

    if (DEBUG_DEV) {
      auto time = millis() - start;
      Serial.print(F("[TIME] Milliseconds: "));
      Serial.println(time);
      Serial.println(F("[INIT] Api Status: connected"));
    }
    Serial.println(F("DONE"));
  } else {
    if (DEBUG_DEV) {
      Serial.println(F("[ERROR] Status: Connection to API failed"));
    }
    Serial.println(F("FAIL"));
  }
}

// Callback in case of an error
void ICACHE_FLASH_ATTR errorCallback(cmd_error * e) {
  CommandError cmdError(e); // Create wrapper object

  Serial.print(F("[ERROR] Status: "));
  Serial.println(cmdError.toString());

  if (cmdError.hasCommand()) {
    Serial.print(F("Did you mean \""));
    Serial.print(cmdError.getCommand().toString());
    Serial.println(F("\"?"));
  }
}

void ICACHE_FLASH_ATTR cli_init() {
  Serial.begin(9600);
  if (DEBUG_DEV) {
    Serial.println(F("[INIT] Initialize Wifi with: DHCP"));
  }
  cli.setOnError(errorCallback); // Set error Callback
  api = cli.addBoundlessCommand("api", apiCallback);
  config = cli.addBoundlessCommand("config", configCallback);
  wifi = cli.addBoundlessCommand("wifi", wifiCallback);
  connect = cli.addBoundlessCommand("connect", connectCallback);
  rest = cli.addBoundlessCommand("rest", restCallback);
  server = cli.addBoundlessCommand("server", serverCallback);
  event = cli.addBoundlessCommand("event", eventCallback);
  type = cli.addBoundlessCommand("type", typeCallback);
  license = cli.addBoundlessCommand("license", licenseCallback);
  verify = cli.addBoundlessCommand("verify", verifyCallback);
  verifier = cli.addBoundlessCommand("verifier", verifierCallback);
  dimensions = cli.addBoundlessCommand("array", dimensionCallback);
  task = cli.addBoundlessCommand("task", taskCallback);
  timer = cli.addBoundlessCommand("timer", timerCallback);
  eeprom = cli.addBoundlessCommand("eeprom", eepromCallback);
  web = cli.addBoundlessCommand("web", webCallback);
  if (DEBUG_DEV) {
    Serial.println(F("[INIT] Status: ready"));
  }
}

void cli_loop() {
  api_client.setNoDelay(true);
  ssl_client.setNoDelay(true);
  if (Serial.available()) {
    // Read out string from the serial monitor
    String input = Serial.readStringUntil('\n');
    if (DEBUG_DEV) {
      Serial.print(F(">> "));
      Serial.println(input);
    }
    // Parse the user input into the CLI
    cli.parse(input);
    if (DEBUG_DEV) {
      Serial.printf("[HEAP] Mem Free: %d\n", free);
      Serial.printf("[HEAP] Free Heap: %d\n", ESP.getFreeHeap());
      Serial.printf("[HEAP] Fragmentation: %d\n", ESP.getHeapFragmentation());
      Serial.printf("[HEAP] Max Free Heap: %d\n", ESP.getMaxFreeBlockSize());
      Serial.println(F("DONE"));
    }
  }

  if (cli.errored()) {
    CommandError cmdError = cli.getError();
    if (DEBUG_DEV) {
      Serial.print(F("[ERROR] Status: "));
      Serial.println(cmdError.toString());
    }
    if (cmdError.hasCommand()) {
      if (DEBUG_DEV) {
        Serial.print(F("Did you mean \""));
        Serial.print(cmdError.getCommand().toString());
        Serial.println(F("\"?"));
      }
    }
  }
}


#endif
