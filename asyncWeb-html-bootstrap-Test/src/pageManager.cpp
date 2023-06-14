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

    serveStaticFiles("/weblib/");
    serveStaticFiles("/web/");
    pushLoginPage();
    pushAdminPage();
    _server->onNotFound([](AsyncWebServerRequest *request){
            request->send(404, "text/plain", "Not found"); 
        });

    _server->begin();
    taskYIELD();
}

pageManager::~pageManager()
{
}

// private:
user_permission pageManager::translatePermission(String permission)
{
    if (permission == "WORKER")
    {
        return WORKER;
    }
    else if (permission == "ADMIN")
    {
        return ADMIN;
    }
    else if (permission == "CALIBRATOR")
    {
        return CALIBRATOR;
    }
    else
    {
        return NO_PERMITION;
    }
    taskYIELD();
}

String pageManager::generateSessionId()
{
    String sessionId = "";
    for (int i = 0; i < 32; i++)
    {
        sessionId += String(random(0, 16), HEX);
    }
    taskYIELD();
    return sessionId;
}

user_permission pageManager::checkUser(String name, String password)
{
    String uerPermission = _userDatabaze->getRecordCell(name.c_str(), "permission");
    String userPassword = _userDatabaze->getRecordCell(name.c_str(), "password");
    if ((uerPermission != "") && (password != ""))
    {
        if (userPassword == password)
        {
            return translatePermission(uerPermission);
        }
    }
    taskYIELD();
    return NO_PERMITION;
}

void pageManager::saveSession(String sessionID, String name, user_permission permition){
    session_Identifier_t session;
    session.name = name;
    session.sessionID = sessionID;
    session.permission = permition;
    _sessions.push_back(session);
    printf("saveSession: %s | %s | %s\n", session.name.c_str(), session.sessionID.c_str(), String(session.permission).c_str());
}

user_permission pageManager::getSessionPermission(AsyncWebServerRequest *request){
    String sessionId = "";
    if (request->hasHeader("Cookie"))
    {
        String cookie = request->getHeader("Cookie")->value();
        int start = cookie.indexOf("sessionId=");
        if (start != -1)
        {
            start += 10;
            int end = cookie.indexOf(";", start);
            if (end == -1)
            {
                end = cookie.length();
            }
            sessionId = cookie.substring(start, end);
        }
    }
    for (int i = 0; i < _sessions.size(); i++)
    {
        if (_sessions[i].sessionID == sessionId)
        {
            return _sessions[i].permission;
        }
    }
    taskYIELD();
    return NO_PERMITION;
}

void pageManager::servFileWithPermission(String path, String fileName, String contentType, const user_permission permission){
    bool noNeedPermission = (permission == NO_PERMITION) ? true : false;
    _server->on(fileName.c_str(), HTTP_GET, [&, path, fileName, contentType, permission, noNeedPermission](AsyncWebServerRequest *request) {
            if ((getSessionPermission(request) == permission) || (noNeedPermission)) {
                request->send(LittleFS, path+fileName, contentType);
            } else {
                request->redirect("/login");
                printf("no permission\n");
            }
        });
}

void pageManager::pushLoginPage()
{
    _server->on("/login", HTTP_GET, [&](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/signin/signin.html", "text/html");
    });
    _server->on("/signin.js", HTTP_GET, [&](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/signin/signin.js", "application/javascript");
    });

    _server->on("/login", HTTP_POST, 
        [](AsyncWebServerRequest *request) {},
        NULL,
        [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            cJSON *pars_data = cJSON_Parse((char *)data);
            String username = cJSON_GetObjectItem(pars_data, "username")->valuestring;
            String password = cJSON_GetObjectItem(pars_data, "password")->valuestring;
            
            String requestBody = String((char*)data);
            printf("%s\n", requestBody.c_str());

            if (username != "" && password != "") {
                printf("uesrname: %s | password: %s \n", username.c_str(), password.c_str());
                
                user_permission permition = checkUser(username, password);
                
                if (permition != NO_PERMITION) {
                    String sessionId = generateSessionId();
                    saveSession(sessionId, username, permition);

                    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", String("{\"type\":\"login\",\"user\":\"admin\"}"));
                    response->addHeader("Set-Cookie", "sessionId=" + sessionId + "; Path=/; HttpOnly");
                    request->send(response);
                    // AsyncWebServerResponse *response = request->beginResponse(200);
                    // response->addHeader("Set-Cookie", "sessionId=" + sessionId + "; Path=/; HttpOnly");
                    // request->send(response);
                    // request->send(200, "application/json", String("{\"type\":\"login\",\"user\":\"admin\"}"));
                    // _ws->textAll("{\"type\":\"login\",\"user\":\"admin\"}");
                }
                else {
                    request->send(401, "text/plain", "Unauthorized");
                }
            }
        }
    );
    taskYIELD();
}

void pageManager::processReport(uint8_t *json)
{
    static user_permission state = NO_PERMITION;
    user_permission newState;

    cJSON pars_data = *cJSON_Parse((char *)json);
  
    switch (state)
    {
    case NO_PERMITION:
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
        case NO_PERMITION:
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