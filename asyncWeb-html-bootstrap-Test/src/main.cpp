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

#include "tunnel.hpp"


/* Put IP Address details */
// int progress_bar_value = 0;
IPAddress local_IP(192,168,0,171);
IPAddress gateway(192, 168,0,99);
IPAddress AP_IP(192,168,4,1);
IPAddress AP_gateway(192, 168,4,2);

IPAddress subnet(255, 255, 0, 0);


// databazeManager userList("/users.csv", USER_LIST);
// databazeManager wifiList("/wifi_network.csv", WIFI_LIST);


// AsyncWebSocketClient * globalClient = NULL;
// ************************************************************************************************************************************************************************************
#define FORMAT_LITTLEFS_IF_FAILED true


void setup(){
  csvDatabese userList("/users.csv");
  csvDatabese wifiList("/wifi_network.csv");
  printf("\nprint user list\n");
  userList.printDatabase();
  printf("\n print wifi list\n");
  wifiList.printDatabase();
  printf("\n");

  // if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
  //   Serial.println("LittleFS Mount Failed");
  //   return;
  // }

  if(!WiFi.config(local_IP, gateway, subnet)) {
    printf("STA Failed to configure\n");
  }
  
  wifiNetworkManager wifiManager(&wifiList);

  std::vector<csvDatabese *> csvDatabeses;
  csvDatabeses.push_back(&wifiList);
  csvDatabeses.push_back(&userList);
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
  delay(1000);
}

// void setup(){
//   Serial.begin(115200);

//   if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
//     Serial.println("LittleFS Mount Failed");
//     return;
//   }

//   if(!WiFi.config(local_IP, gateway, subnet)) {
//     printf("STA Failed to configure\n");
//   }

//   WiFi.begin("Vodafone-AEA0", "WS677xkrpuh2arAe");
//   WiFi.softAP("my_WiFi", "12345678");

//   // Print ESP32 Local IP Address
//   Serial.print("WiFi.localIP(): ");
//   Serial.println(WiFi.localIP());
//   Serial.print("WiFi.softAPIP(): ");
//   Serial.println(WiFi.softAPIP());
//   delay(1000);

//   TunnelTcpUart tunnel(1234, 0, 115200);
//   tunnel.start();

// }



// WiFiServer server(1234);
// WiFiClient clientA;
// WiFiClient clientB;

// void setup(){
//   Serial.begin(115200);
//   printf("Start\n");


//   // Připojení k existující WiFi síti
//   WiFi.begin("Vodafone-AEA0", "WS677xkrpuh2arAe");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     printf("Připojování k WiFi...\n");
//   }

//   printf("Připojeno k WiFi\n");

//   // Vytvoření přístupového bodu (AP)
//   WiFi.softAP("my_WiFi", "12345678");
//   delay(1000);

//   printf("WiFi.localIP(): %s\n", WiFi.localIP().toString().c_str());
//   printf("WiFi.softAPIP(): %s\n", WiFi.softAPIP().toString().c_str());
//   delay(1000);

//   // Spuštění TCP serveru
//   server.begin();
//   printf("Server spuštěn");
// }

// void loop() {
//   char clientDataA;
//   char clientDataB;
//   bool newA = false;
//   bool newB = false;

//   // Příjem připojení od klientů
//   if (!clientA.connected()) {
//     clientA = server.available();
//   }

//   if (!clientB.connected()) {
//     clientB = server.available();
//   }

//   // Přenos dat ze zařízení A na zařízení B
//   if (clientA && clientA.connected()) {
//     printf("clientA.localIP(): %s\n", clientA.localIP().toString().c_str());
//     if (clientA.available()) {
//       clientDataA = clientA.read();
//       clientA.write(clientDataA);
//       newA = true;
//       // Serial.print("Přijato od zařízení A: ");
//       Serial.println(clientDataA);
//     }
//   }

//   // Přenos dat ze zařízení B na zařízení A
//   if (clientB && clientB.connected()) {
//     if (clientB.available()) {
//       clientDataB = clientB.read();
//       clientB.write(clientDataB);
//       newB = true;
//       // Serial.print("Přijato od zařízení B: ");
//       Serial.println(clientDataB);
//     }
//   }

//   // Odesílání dat mezi klienty
//   if (clientA && clientA.connected() && newB) {
//     clientA.write(clientDataB);
//     newB = false;
//   }

//   if (clientB && clientB.connected() && newA) {
//     clientB.write(clientDataA);
//     newA = false;
//   }
// }
