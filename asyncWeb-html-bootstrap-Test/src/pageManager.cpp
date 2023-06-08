#include "pageManager.hpp"

pageManager *pageManager::_instance = nullptr;

pageManager::pageManager(wifiNetworkManager *wifiManager, csvDatabese *userDatabaze, std::vector<csvDatabese *>  *databazes)
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
    taskYIELD();
}

pageManager::~pageManager()
{
}

// private:
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
    taskYIELD();
}

machineStates pageManager::loginProcess(cJSON& json)
{
    if(cJSON_GetObjectItem(&json, "username")->valuestring != NULL)
    {
        String uerPermission = _userDatabaze->getRecordCell(cJSON_GetObjectItem(&json, "username")->valuestring, "permission");
        if (uerPermission != "")
        {
            return translatePermission(uerPermission);
        }
    }
    taskYIELD();
    return LOGIN;
}

void pageManager::pushLoginPage()
{
    printf("pushLoginPage\n");
    taskYIELD();
}

void pageManager::processReport(uint8_t *json)
{
    static machineStates state = LOGIN;
    machineStates newState;

    cJSON pars_data = *cJSON_Parse((char *)json);
    
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
            pushLoginPage();
            break;
        case WORKER:
            printf("go to WORKER\n");
            pushWorkerPage();
            break;
        case ADMIN:
            printf("go to ADMIN\n");
            pushAdminPage();
            break;
        case CALIBRATOR:
            printf("go to CALIBRATOR\n");
            pushCalibratorPage();
            break;
        }
    }
    taskYIELD();
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
    taskYIELD();
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
    taskYIELD();
}