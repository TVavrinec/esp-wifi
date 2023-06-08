#include "pageManager.hpp"

machineStates pageManager::adminProcess(cJSON& json)
{
    String type = cJSON_GetObjectItem(&json, "type")->valuestring;
    if(type == "wifi")
    {
        wifiEvent(json);
    }
    else if(type == "general")
    {
        printf("general\n");
        sendWifiList();
    }
    else
    {
        printf("Wrong: Unknown packet type\n");
    }
    taskYIELD();
    return ADMIN;
}

/**
 * @brief push admin page to all clients
*/
void pageManager::pushAdminPage()
{
    serveStaticFiles("/users/admin/");
    serveStaticFiles("/web/");
    _ws->textAll("{\"type\":\"login\",\"user\":\"admin\"}");    
    taskYIELD();
}

/**
 * @brief get wifi packet from websocket and process it
*/
void pageManager::wifiEvent(cJSON& json)
{
    cJSON *ap = cJSON_GetObjectItem(&json, "ap");
    if(ap != NULL){
        cJSON *ssid = cJSON_GetObjectItem(ap, "ssid");
        cJSON *on = cJSON_GetObjectItem(ap, "on");
        if(ssid != NULL){

            // _wifiManager->startAP();
        }
        else if(on != NULL){
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
    taskYIELD();
}


void pageManager::sendWifiList(){
    wifiNetwork connectedWifi = _wifiManager->getConnectedWifiNetwork();

    std::vector<wifiNetwork> availableWifiList = _wifiManager->getAvailableWifiList();

    cJSON *message, *station, *wifiList, *wifi;

    message = cJSON_CreateObject();
    cJSON_AddItemToObject(message, "type", cJSON_CreateString("wifi"));

    station = cJSON_CreateObject();
    cJSON_AddItemToObject(message, "station", station);

    wifiList = cJSON_CreateArray();
    cJSON_AddItemToObject(station, "wifiList", wifiList);

    wifi = cJSON_CreateObject();
    cJSON_AddStringToObject(wifi, "ssid", connectedWifi.ssid.c_str());
    cJSON_AddNumberToObject(wifi, "rssi", connectedWifi.rssi);
    cJSON_AddStringToObject(wifi, "state", "connected");
    cJSON_AddItemToArray(wifiList, wifi);

    for (wifiNetwork& network : availableWifiList) {
        wifi = cJSON_CreateObject();
        cJSON_AddStringToObject(wifi, "ssid", network.ssid.c_str());
        cJSON_AddNumberToObject(wifi, "rssi", network.rssi);
        cJSON_AddStringToObject(wifi, "state", "not_connected");
        cJSON_AddItemToArray(wifiList, wifi);
        taskYIELD();
    }

    char *json_string = cJSON_Print(message);
    _ws->textAll(json_string);
    taskYIELD();
}

