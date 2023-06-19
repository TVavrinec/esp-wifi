#include "pageManager.hpp"

pageManager *pageManager::_instance = nullptr;

pageManager::pageManager(wifiNetworkManager *wifiManager, csvDatabese *userDatabaze, std::vector<csvDatabese *>  *databazes)
{
    _wifiManager = wifiManager;
    _userDatabaze = userDatabaze;
    _databazes = databazes;
    
    _instance = this;
    
    // static AsyncWebServer server(80);
    // static AsyncWebSocket ws("/ws");
    _server = new AsyncWebServer(80);
    _ws = new AsyncWebSocket("/ws");

    _ws->onEvent(onWsEvent);
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
/**
 * @brief push admin page to all clients
 */
void pageManager::pushAdminPage()
{
    servFileWithPermission("/users/admin", "/admin.html", "text/html", ADMIN);
    servFileWithPermission("/users/admin", "/adminScript.js", "application/javascript", ADMIN);
    servFileWithPermission("/users/admin", "/settings.js", "application/javascript", ADMIN);
    servFileWithPermission("/users/admin", "/wifi-high-svgrepo-com.svg", "image/svg+xml", ADMIN);
    taskYIELD();
}

/**
 * @param String version of permission
 * @return user_permission permission
 */
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

/**
 * @brief generate session id
 * 
 * @return String session id
 */
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

/**
 * @brief check if is user in database with this password
 * 
 * @param name - user name
 * @param password - user password
 * @return user_permission - user permission
 */
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

/**
 * @brief save session to session list
 * 
 * @param sessionID - session id
 * @param name - user name
 * @param permition - user permission
 */
void pageManager::saveSession(String sessionID, String name, user_permission permition){
    session_Identifier_t session;
    session.name = name;
    session.sessionId = sessionID;
    session.permission = permition;
    _sessions.push_back(session);
}

/**
 * @brief get permission from session id
 * 
 * @param request - request from client
 * @return user_permission - user permission
 */
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
        if (_sessions[i].sessionId == sessionId)
        {
            return _sessions[i].permission;
        }
    }
    taskYIELD();
    return NO_PERMITION;
}

/**
 * @brief serves a file with the specified permission level
 * 
 * @param path - path to file in LittleFS
 * @param fileName - file name
 * @param contentType - content type
 * @param permission - permission level
 */
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

/**
 * @brief push login page to all clients
 */
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

            if (username != "" && password != "") {                
                user_permission permition = checkUser(username, password);
                
                if (permition != NO_PERMITION) {
                    String sessionId = generateSessionId();
                    saveSession(sessionId, username, permition);

                    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", String("{\"type\":\"login\",\"user\":\"admin\"}"));
                    response->addHeader("Set-Cookie", "sessionId=" + sessionId + "; Path=/; HttpOnly");
                    request->send(response);
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
    cJSON pars_data = *cJSON_Parse((char *)json);
    
    cJSON *packet_type = cJSON_GetObjectItem(&pars_data, "type");
    if(packet_type != NULL)
    {
        String type = packet_type->valuestring;
        if (type == "wifi")
        {
            wifiEvent(pars_data);
        }
        else if (type == "general")
        {
            sendWifiList();
        }
        else
        {
            printf("Wrong: Unknown packet type\n");
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
            }
            file.close();
            file = root.openNextFile();
        }
    }
    taskYIELD();
}

void pageManager::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    printf("onWsEventtype = %d\n", type);
    if (type == WS_EVT_CONNECT)
    {
        Serial.println("WS_EVT_CONNECT\n");
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.println("WS_EVT_DISCONNECT\n");

    }
    else if (type == WS_EVT_DATA)
    {
        _instance->processReport(data);
    }
    else{
        printf("Unknown event\n");
    }
    taskYIELD();
}