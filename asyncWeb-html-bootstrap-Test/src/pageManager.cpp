#include "pageManager.hpp"

pageManager *pageManager::_instance = nullptr;

pageManager::pageManager(wifiNetworkManager *wifiManager, csvDatabese *userDatabaze, std::vector<csvDatabese>  *databazes)
{
    _wifiManager = wifiManager;
    _userDatabaze = userDatabaze;
    _databazes = databazes;
    
    _instance = this;
    
    static AsyncWebServer server(80);
    static AsyncWebSocket ws("/ws");
    _server = &server;
    _ws = &ws;

    _ws->onEvent(&onWsEvent);
    _server->addHandler(_ws);

    serveStaticFiles("/signin/");
    serveStaticFiles("/weblib/");
    _server->onNotFound([](AsyncWebServerRequest *request)
                        { request->send(404, "text/plain", "Not found"); });

    _server->begin();
}

pageManager::~pageManager()
{
}

// private:
DynamicJsonDocument pageManager::parsReport(char *json)
{
    DynamicJsonDocument parsJson(1024);

    if(deserializeJson(parsJson, json)){
        printf("ERROR: Failed to deserialize json\n");
    }
    return parsJson;
}

machineStates pageManager::translatePermission(String permission)
{
    printf("test translatePermission\n");
    printf("translatePermission: %s\n", permission.c_str());
    if (permission == "WORKER")
    {
        printf("WORKER\n");
        return WORKER;
    }
    else if (permission == "ADMIN")
    {
        printf("ADMIN\n");
        return ADMIN;
    }
    else if (permission == "CALIBRATOR")
    {
        printf("CALIBRATOR\n");
        return CALIBRATOR;
    }
    else
    {
        printf("LOGIN\n");
        return LOGIN;
    }
}

machineStates pageManager::loginProcess(DynamicJsonDocument& json)
{
    if (json["username"])
    {
        String uerPermission = _userDatabaze->getRecordCell(json["username"], "permission");
        if (uerPermission != "")
        {
            return translatePermission(uerPermission);
        }
    }
    return LOGIN;
}

machineStates pageManager::workerProcess(DynamicJsonDocument& json)
{

    return WORKER;
}

machineStates pageManager::adminProcess(DynamicJsonDocument& json)
{
    if(json["type"] == "wifi")
    {
        wifiEvent(json);
    }
    else
    {
        printf("Wrong: Unknown packet type\n");
    }
    
    return ADMIN;
}

machineStates pageManager::calibratorProcess(DynamicJsonDocument& json)
{

    return CALIBRATOR;
}

void pageManager::pushLoginPage(DynamicJsonDocument& json)
{
    printf("pushLoginPage\n");
}

void pageManager::pushWorkerPage(DynamicJsonDocument& json)
{
    serveStaticFiles("/users/worker/");
    serveStaticFiles("/web/");
    _ws->textAll("{\"type\":\"login\",\"user\":\"worker\"}");
}

void pageManager::pushAdminPage(DynamicJsonDocument& json)
{
    serveStaticFiles("/users/admin/");
    serveStaticFiles("/web/");
    _ws->textAll("{\"type\":\"login\",\"user\":\"admin\"}");
}

void pageManager::pushCalibratorPage(DynamicJsonDocument& json)
{
    serveStaticFiles("/users/calibrator/");
    serveStaticFiles("/web/");
    _ws->textAll("{\"type\":\"login\",\"user\":\"calibrator\"}");
}

/**
 * @brief get wifi packet from websocket and process it
*/
void pageManager::wifiEvent(DynamicJsonDocument& json)
{
    String apAction = json["ap"];
    if(json["ap"].is<const char*>()){
        if(apAction == "apOff"){
            // _wifiManager->startAP();
        }
        else if(apAction == "apOff"){
            // _wifiManager->stopAP();
        }
        // else if(apAction == "restart"){
        //     _wifiManager->restartAP();
        // }
        // else if(apAction == "change"){
        //     String ssid = getPacketPart(data, "ssid");
        //     String password = getPacketPart(data, "password");
        //     _wifiManager->changeWifi(ssid, password);
        // }
    }

    // String wifi[2] = {"", ""};
    // for (int a = 0; a < data->members[0].size(); a++)
    // {
    //     if (data->members[0][a] == "ssid")
    //     {
    //         wifi[0] = data->members[1][a];
    //     }
    //     else if (data->members[0][a] == "password")
    //     {
    //         wifi[1] = data->members[1][a];
    //     }
    // }
    // if (wifi[0] == "" || wifi[1] == "")
    // {
    //     _wifiManager->changeWifi(wifi[0], wifi[1]);
    // } 
}

void pageManager::processReport(uint8_t *json)
{
    static machineStates state = LOGIN;
    machineStates newState;

    DynamicJsonDocument pars_data = parsReport((char *)json);
    
    switch (state)
    {
    case LOGIN:
        newState = loginProcess(pars_data);
        break;
    case WORKER:
        newState = workerProcess(pars_data);
        break;
    case ADMIN:
        newState = adminProcess(pars_data);
        break;
    case CALIBRATOR:
        newState = calibratorProcess(pars_data);
        break;
    }
    if (state != newState)
    {
        state = newState;
        switch (state)
        {
        case LOGIN:
            printf("go to LOGIN\n");
            pushLoginPage(pars_data);
            break;
        case WORKER:
            printf("go to WORKER\n");
            pushWorkerPage(pars_data);
            break;
        case ADMIN:
            printf("go to ADMIN\n");
            pushAdminPage(pars_data);
            break;
        case CALIBRATOR:
            printf("go to CALIBRATOR\n");
            pushCalibratorPage(pars_data);
            break;
        }
    }
}

void pageManager::serveStaticFiles(const char *path)
{
    File root = LittleFS.open(path, "r");
    if (root)
    {
        File file = root.openNextFile();
        while (file)
        {
            String filePath = String(path) + file.name();
            if (LittleFS.exists(filePath))
            {
                _server->serveStatic(file.name(), LittleFS, filePath.c_str());
                printf("serveStatic: %s\n", filePath.c_str());
            }
            file.close();
            file = root.openNextFile();
        }
    }
}

void pageManager::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    printf("onWsEvent\n");
    if (type == WS_EVT_CONNECT)
    {
        Serial.println("WS_EVT_CONNECT");
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.println("WS_EVT_DISCONNECT");
    }
    else if (type == WS_EVT_DATA)
    {
        _instance->processReport(data);
    }
}