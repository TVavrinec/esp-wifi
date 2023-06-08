#pragma once

#include <WiFi.h>
#include <iostream>
#include <vector>
#include <string>

#include "csvDatabese.hpp"

typedef enum { 
    CONNECTED,
    UNKNOW_WIFI,
    NO_WIFI,
    UNKNOW_PACKET
} wifi_result_t;

struct wifiNetwork{
    String ssid;
    int rssi;
};

/*
* This class is used to manage wifi network.
* It can work with csvDatabaze class and automatically create AP if there is no wifi network.
* It supposed format of csvDatabaze class:
* 
*
* | SSID     | PASSWORD  |
* |----------|-----------| 
* |AP WiFi   |AP Password|
* |other WiFi| Passworld |
* |other WiFi| Passworld |
* |other WiFi| Passworld |
* |...       | ...       |
*   
*/
class wifiNetworkManager
{
private:
    std::mutex _mutex__;

    String _AP_ssid;
    String _AP_password;
    String _connectedWifiNetwork;
    String _connectedWifiPassword;
    csvDatabese *_wifiDatabaze;
    String getAvailableWifiFromDatabaze();

    void _startAP(const char* AP_ssid = "Exactis-SMC", const char* AP_password = "12345678");

public:
    wifi_result_t changeWifi(String ssid, String password);
    wifi_result_t changeWifi(String ssid);
    
    void startStation();
    void stopStation();

    wifiNetwork getConnectedWifiNetwork();
    std::vector<wifiNetwork> getAvailableWifiList();

    void restartAP(const char* AP_ssid = "Exactis-SMC", const char* AP_password = "12345678");
    void stopAP();
    void startAP();

    wifiNetworkManager(csvDatabese *wifiDatabaze, bool AP_always_on = true, const char* AP_ssid = "Exactis-SMC", const char* AP_password = "12345678");
    ~wifiNetworkManager();
};
