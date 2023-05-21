// #include <WiFi.h>
// #include <ESPAsyncWebServer.h>
// #include <FS.h>
// #include <LittleFS.h>
// #include <time.h>
// #include <iostream>
// #include <vector>
// #include <string>

// #include <USBHost.h>

#include "csvDatabese.hpp"
#include "pageManager.hpp"
#include "wifiNetworkManager.hpp"

// #include "tunnel.hpp"


/* Put IP Address details */
// int progress_bar_value = 0;
IPAddress local_IP(192,168,0,171);
IPAddress gateway(192, 168,0,99);
IPAddress AP_IP(192,168,4,1);
IPAddress AP_gateway(192, 168,4,1);


IPAddress subnet(255, 255, 0, 0);


// databazeManager userList("/users.csv", USER_LIST);
// databazeManager wifiList("/wifi_network.csv", WIFI_LIST);


// AsyncWebSocketClient * globalClient = NULL;
// ************************************************************************************************************************************************************************************
#define FORMAT_LITTLEFS_IF_FAILED true

void setup(){
  Serial.begin(115200);
  Serial.println("Start");
  csvDatabese userList("/users.csv");
  csvDatabese wifiList("/wifi_network.csv");
  Serial.println("Start printing");
  userList.printDatabase();
  Serial.println("\n");
  wifiList.printDatabase();
  Serial.println("\nEnd printing\n");

  // if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
  //   Serial.println("LittleFS Mount Failed");
  //   return;
  // }

  if(!WiFi.config(local_IP, gateway, subnet)) {
    printf("STA Failed to configure\n");
  }
  
  wifiNetworkManager wifiManager(&wifiList);

  std::vector<csvDatabese> csvDatabeses;
  csvDatabeses.push_back(wifiList);
  csvDatabeses.push_back(userList);
  pageManager page(&wifiManager, &userList, &csvDatabeses);

  // WiFi.begin("Vodafone-AEA0", "WS677xkrpuh2arAe");
  // WiFi.softAP("my_WiFi", "12345678");

  // // Print ESP32 Local IP Address
  // Serial.print("WiFi.localIP(): ");
  // Serial.println(WiFi.localIP());
  // Serial.print("WiFi.softAPIP(): ");
  // Serial.println(WiFi.softAPIP());
  // delay(1000);

  while(1){
    delay(10000);
  }
}

void loop(){
}
