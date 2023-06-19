#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define BUFFER_SIZE 1024

class TunnelTcpUart
{
private:
    HardwareSerial *_uart;
    AsyncServer *_server;
    IPAddress *_clientId;
    AsyncClient  *_client;

public:
    TunnelTcpUart(int tcpPort, HardwareSerial uart, IPAddress *clientId);
    ~TunnelTcpUart();

    void start();
    void end();
};


// class TunnelTcpTcp
// {
// private:
//     int _tcpPort0;
//     int _tcpPort1;
//     uint8_t _buff[BUFFER_SIZE];

//     TaskHandle_t _tunnel = NULL;

//     void loop(WiFiClient *clientWifiTunnel, HardwareSerial *localUart);
//     static void tunnelProcess(void *self_);

// public:
//     TunnelTcpTcp(int tcpPort0, int tcpPort1);

//     void start();
//     void end();
// };


// prototype *******************************************************************************************************************************************
// #include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <vector>

// // Create AsyncServer object on port 1234
// AsyncServer server(1234);

// // Target IP address
// IPAddress targetIP(192, 168, 4, 2); // example, replace with the IP of your target client

// std::vector<AsyncClient*> clients;

// void setup(){
//   Serial0.begin(115200);
//   WiFi.mode(WIFI_AP_STA);
//   WiFi.begin("Vodafone-AEA0","WS677xkrpuh2arAe");
//   WiFi.softAP("my_WiFi", "12345678");

//   // Wait for the Wi-Fi to connect
//   while (WiFi.status() != WL_CONNECTED) {
//       delay(1000);
//       Serial.println("Connecting to WiFi...");
//   }

//   printf("softAPIP(): %s\n", WiFi.softAPIP().toString().c_str());
//   printf("localIP(): %s\n", WiFi.localIP().toString().c_str());


//   // Start server
//   server.begin();
  
//   // Handle new clients
//   server.onClient([](void *h, AsyncClient* client){
//     clients.push_back(client);
//     // targetIP = client->remoteIP();
//     // printf("New client IP: %s\n", targetIP.toString().c_str());
//     printf("New client IP: %s\n", client->remoteIP().toString().c_str());

//     // Handle client disconnection
//     client->onDisconnect([](void *h, AsyncClient* client){
//       clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
//       delete client;
//     }, NULL);

//     // Handle incoming data
//     client->onData([](void *obj, AsyncClient* c, void *data, size_t len){
//       if(c->remoteIP() == targetIP){
//         Serial0.write((uint8_t*)data, len);
//       }
//     }, NULL);

//   }, NULL);
// }

// void loop(){
//   if (Serial0.available()) {
//     char data = Serial0.read();
//     for (AsyncClient* client : clients) {
//       if (client->remoteIP() == targetIP) {
//         client->write(&data, 1);
//       }
//     }
//   }

//   delay(100);
// }
