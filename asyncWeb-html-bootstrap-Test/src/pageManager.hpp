#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <time.h>
// #include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <ArduinoJson.h>

#include "csvDatabese.hpp"
#include "wifiNetworkManager.hpp"

typedef enum { 
  LOGIN, 
  WORKER, 
  ADMIN, 
  CALIBRATOR
} machineStates;

typedef enum { 
  WIFI,
  SIGN_IN
} packet_type_t;

/**
 * @brief Class for managing pages and user permissions
 * 
 * @details This class need to be initialized with all csvDatabaze in memory class and wifiNetworkManager class.
*/
class pageManager
{
private:
    static pageManager* _instance;

    AsyncWebServer *_server;
    AsyncWebSocket *_ws;
    csvDatabese *_userDatabaze;
    wifiNetworkManager *_wifiManager;
    std::vector<csvDatabese>  *_databazes;

    DynamicJsonDocument parsReport(char *json);

    machineStates translatePermission(String permission);

    machineStates loginProcess(DynamicJsonDocument& json);
    machineStates workerProcess(DynamicJsonDocument& json);
    machineStates adminProcess(DynamicJsonDocument& json);
    machineStates calibratorProcess(DynamicJsonDocument& json);

    String getPacketPart(DynamicJsonDocument& json, String partName);

    void serveStaticFiles(const char *path);

    void processReport(uint8_t *data);
    
    // void signinEvent(DynamicJsonDocument& json);
    void wifiEvent(DynamicJsonDocument& json);

    void pushLoginPage(DynamicJsonDocument& json);
    void pushWorkerPage(DynamicJsonDocument& json);
    void pushAdminPage(DynamicJsonDocument& json);
    void pushCalibratorPage(DynamicJsonDocument& json);


    static void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

public:
    pageManager(wifiNetworkManager *wifiManager, csvDatabese *userDatabaze, std::vector<csvDatabese> *databaze);
    ~pageManager();
};
