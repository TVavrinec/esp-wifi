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
  NO_PERMITION, 
  WORKER, 
  ADMIN, 
  CALIBRATOR
} user_permission;

typedef enum { 
  WIFI,
  SIGN_IN
} packet_type_t;

typedef struct {
  String name;
  String sessionID;
  user_permission permission;
} session_Identifier_t;

/**
 * @brief Class for managing pages and user permissions
 * 
 * @details This class need to be initialized with all csvDatabaze in memory class and wifiNetworkManager class.
*/
class pageManager
{
private:
    static pageManager* _instance;
    std::vector<session_Identifier_t> _sessions;

    AsyncWebServer *_server;
    AsyncWebSocket *_ws;
    csvDatabese *_userDatabaze;
    wifiNetworkManager *_wifiManager;
    std::vector<csvDatabese *>  *_databazes;

    cJSON parsReport(char *json);

    user_permission translatePermission(String permission);

    String getPacketPart(cJSON & json, String partName);
    
    void serveStaticFiles(const char *path);

    void processReport(uint8_t *data);
    
    static void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

    user_permission getSessionPermission(AsyncWebServerRequest *request);
    void servFileWithPermission(String path, String fileName, String contentType,const user_permission permission);
    
    // Login
    void pushLoginPage();
    user_permission checkUser(String name, String password);
    String generateSessionId();
    void saveSession(String sessionID, String name, user_permission permission);

    // Calibrator
    user_permission calibratorProcess(cJSON & json);
    void pushCalibratorPage();

    // Worker
    user_permission workerProcess(cJSON & json);
    void pushWorkerPage();

    // Admin
    user_permission adminProcess(cJSON & json);
    void pushAdminPage();

    void wifiEvent(cJSON & json);
      void apEvent(cJSON & json);
      void stationEvent(cJSON & json);
        void stationGetEvent(cJSON & json);
        void stationOnEvent(cJSON & json);
        void stationSsidEvent(cJSON & json);

    void sendWifiList();
    
    // void signinEvent(cJSON & json);

public:
    pageManager(wifiNetworkManager *wifiManager, csvDatabese *userDatabaze, std::vector<csvDatabese *> *databaze);
    ~pageManager();
};
