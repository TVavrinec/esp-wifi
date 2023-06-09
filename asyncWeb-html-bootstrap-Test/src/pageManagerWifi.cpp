#include "pageManager.hpp"

/**
 * @brief get wifi packet from websocket and process it
 */
void pageManager::wifiEvent(cJSON &json)
{
    cJSON *ap = cJSON_GetObjectItem(&json, "ap");
    cJSON *station = cJSON_GetObjectItem(&json, "station");
    if (ap != NULL)
    {
        apEvent(*ap);
        return;
    }
    else if(station != NULL)
    {
        stationEvent(*station);
        return;
    }
    else
    {
        printf("Wrong: Unknown packet type - wifi\n");
    }

    taskYIELD();
}

/**
 * solve ap packet
 */
void pageManager::apEvent(cJSON &json)
{
    cJSON *ssid = cJSON_GetObjectItem(&json, "ssid");
    cJSON *on = cJSON_GetObjectItem(&json, "on");
    if (ssid != NULL)
    {
        cJSON *password = cJSON_GetObjectItem(&json, "password");
        if(password != NULL)
        {
            if(_wifiManager->changeAP(ssid->valuestring, password->valuestring) == SAVED){
                _ws->textAll("{\"type\":\"wifi\",\"ap\":{\"set\":true}}");
            }
            else{
                _ws->textAll("{\"type\":\"wifi\",\"ap\":{\"set\":false}}");
            }
        }
    }
    else if (on != NULL)
    {
        if(on->valueint)
            _wifiManager->startAP();
        else
            _wifiManager->stopAP();
    }
    else
    {
        printf("Wrong: Unknown packet type - wifi/AP\n");
    }
    taskYIELD();
}

/**
 * solve station packet
 */
void pageManager::stationEvent(cJSON &json){
    cJSON *get = cJSON_GetObjectItem(&json, "get");
    cJSON *on = cJSON_GetObjectItem(&json, "on");
    cJSON *ssid = cJSON_GetObjectItem(&json, "ssid");
    if (get != NULL)
    {
        printf("get: %s\n", get->valuestring);
        stationGetEvent(json);
    }
    else if (on != NULL)
    {
        printf("on: %d\n", on->valueint);
        stationOnEvent(json);
    }
    else if (ssid != NULL)
    {
        printf("ssid: %s\n", ssid->valuestring);
        stationSsidEvent(json);
    }
    else
    {
        printf("Wrong: Unknown packet type - wifi/station\n");
    }
    taskYIELD();
}

/**
 * solve station get packet
*/
void pageManager::stationGetEvent(cJSON &json){
    String get = cJSON_GetObjectItem(&json, "get")->valuestring;
    if (get == "wifi_list")
    {
        sendWifiList();
    }
    else
    {
        printf("Wrong: Unknown packet type - wifi/station/get\n");
    }
    taskYIELD();
}

/**
 * solve station on packet
*/
void pageManager::stationOnEvent(cJSON &json){
    // bool on = cJSON_GetObjectItem(&json, "on")->valueint;
    // if (on)
    // {
    //     _wifiManager->startStation();
    // }
    // else
    // {
    //     _wifiManager->stopStation();
    // }
    taskYIELD();
}

/**
 * solve station ssid packet
*/
void pageManager::stationSsidEvent(cJSON &json){
    String ssid = cJSON_GetObjectItem(&json, "ssid")->valuestring;
    cJSON *password = cJSON_GetObjectItem(&json, "password");
    if(password == NULL){
        wifi_save_status_t connectState = _wifiManager->changeActiveWifi(ssid);
        String connectMessage;
        if(connectState == SAVED){
            connectMessage = "known_wifi";
        }
        else if(connectState == UNSAVED){
            connectMessage = "unknown_wifi";
        }
        _ws->textAll("{\"type\":\"wifi\",\"station\":{\"store\":\"" + connectMessage + "\"}}");
    }
    else{
        printf("ssid: %s\n", ssid.c_str());
        if(_wifiManager->changeActiveWifi(ssid, password->valuestring) != SAVED){
            _ws->textAll("{\"type\":\"wifi\",\"station\":{\"store\":\"unknown_wifi\"}}");
        }
        else{
            _ws->textAll("{\"type\":\"wifi\",\"station\":{\"store\":\"kown_wifi\"}}");
        }
    }
    taskYIELD();
}

void pageManager::sendWifiList()
{
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

    for (wifiNetwork &network : availableWifiList)
    {
        wifi = cJSON_CreateObject();
        cJSON_AddStringToObject(wifi, "ssid", network.ssid.c_str());
        cJSON_AddNumberToObject(wifi, "rssi", network.rssi);
        cJSON_AddStringToObject(wifi, "state", "not_connected");
        cJSON_AddItemToArray(wifiList, wifi);
    }

    char *json_string = cJSON_Print(message);
    _ws->textAll(json_string);
    taskYIELD();
}
