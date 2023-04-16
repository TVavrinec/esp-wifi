#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
	
/* Put IP Address details */
int progress_bar_value = 0;
IPAddress local_IP(192,168,0,171);
IPAddress gateway(192, 168,4,1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

 
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
 
AsyncWebSocketClient * globalClient = NULL;
 
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
 
  if(type == WS_EVT_CONNECT){
 
    Serial.println("Websocket client connection received");
    globalClient = client;
 
  } else if(type == WS_EVT_DISCONNECT){
 
    Serial.println("Websocket client connection finished");
    globalClient = NULL;
 
  }
}
 
void setup(){  
  Serial.begin(115200);
  Serial.println("Websocket client connected\n");
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  
  if(!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin("Vodafone-AEA0", "WS677xkrpuh2arAe");
  WiFi.softAP("my_WiFi", "12345678");

  // startWebSocket();            // Start a WebSocket server

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.print("WiFi.localIP(): ");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi.softAPIP(): ");
  Serial.println(WiFi.softAPIP());
 
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Route for root / web page
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String()); 
  });

  server.on("/chart.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/chart.min.js", String());
  });

  server.on("/jquety-3.5.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquety-3.5.1.min.js", String());
  });

  server.on("/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bootstrap.bundle.min.js", String());
  });

  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bootstrap.min.css", String());
  });

  server.on("/popper.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/popper.min.js", String());
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", String());
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css", false);
  });

  // Route to set GPIO to HIGH
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    printf("\ngetParam(0) = %s\n", request->getParam(0));
    printf("getParam(1) = %s\n", request->getParam(1));
    if (request->hasParam("type")) {
      String test = request->getParam("type")->value();
      printf("type: %s\n", test);
      test = request->getParam("id")->value();
      printf("id: %s\n\n", test);
    }
    // printf("\n%s\n",test);
    request->send(SPIFFS, "/index.html", String());
  });
 
  server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/ws.html", "text/html");
  });
 
  server.begin();
  while(1){  
    for(int i = 0; i < 100; i++){
      if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
        String randomNumber = "arr: ," + String(i*i) + "," + String(i) + ",";
        globalClient->text(randomNumber);
      }
      delay(100);
    }
    delay(10000);
  }
}
 
void loop(){
  // if(globalClient != NULL && globalClient->status() == WS_CONNECTED){
  //   String randomNumber = "arr: ," + String(random(0,20)) + "," + String(random(0,20)) + ",";
  //   globalClient->text(randomNumber);
  // }
  delay(10000);
}
