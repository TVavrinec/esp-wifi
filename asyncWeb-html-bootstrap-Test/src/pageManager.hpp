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

#include "cJSON.h"

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
    std::vector<csvDatabese *>  *_databazes;

    cJSON parsReport(char *json);

    machineStates translatePermission(String permission);

    String getPacketPart(cJSON & json, String partName);
    
    void serveStaticFiles(const char *path);

    void processReport(uint8_t *data);
    
    static void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

    // Login
    machineStates loginProcess(cJSON & json);
    void pushLoginPage();

    // Calibrator
    machineStates calibratorProcess(cJSON & json);
    void pushCalibratorPage();

    // Worker
    machineStates workerProcess(cJSON & json);
    void pushWorkerPage();

    // Admin
    machineStates adminProcess(cJSON & json);
    void wifiEvent(cJSON & json);
    void pushAdminPage();

    void sendWifiList();
    
    // void signinEvent(cJSON & json);

public:
    pageManager(wifiNetworkManager *wifiManager, csvDatabese *userDatabaze, std::vector<csvDatabese *> *databaze);
    ~pageManager();
};
