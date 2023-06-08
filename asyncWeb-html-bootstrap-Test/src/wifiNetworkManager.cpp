#include "wifiNetworkManager.hpp"

wifiNetworkManager::wifiNetworkManager(csvDatabese *wifiDatabaze, bool AP_always_on, const char* AP_ssid, const char* AP_password)
{
    _wifiDatabaze = wifiDatabaze;
    String availableWifi = getAvailableWifiFromDatabaze();
    if(availableWifi != ""){
        WiFi.begin(availableWifi.c_str(), _wifiDatabaze->getRecordCell(availableWifi.c_str(), "password").c_str());
        _connectedWifiNetwork = availableWifi;
    }
    else
    {
        AP_always_on = true;
    }
    if(AP_always_on){
        startAP(AP_ssid, AP_password);
    }
    
};

wifiNetworkManager::~wifiNetworkManager()
{
};

// public:  ********************************************************************************************************************   public:

/**
 * Get actual wifi network
 * @return wifiNetwork with actual wifi network
*/
wifiNetwork wifiNetworkManager::getConnectedWifiNetwork()
{
    std::lock_guard<std::mutex> lock(_mutex__);

    _connectedWifiNetwork = WiFi.SSID();
    return wifiNetwork{
        _connectedWifiNetwork, 
        WiFi.RSSI()
    };
};

/**
 * Search for available WiFi networks
 * @param show_hidden   show hidden networks
 * @return list of discovered networks
 */
std::vector<wifiNetwork> wifiNetworkManager::getAvailableWifiList()
{
    std::lock_guard<std::mutex> lock(_mutex__);
    
    int numNetworks = WiFi.scanNetworks();
    std::vector<wifiNetwork> availableWifiList;
    for(int i = 0; i < numNetworks; i++){
        String nextWifi = WiFi.SSID(i);
        if(_connectedWifiNetwork != nextWifi){
            availableWifiList.push_back(wifiNetwork{
                nextWifi, 
                WiFi.RSSI(i)
            });
        }
        taskYIELD();
    }
    return availableWifiList;
};


/**
 * Change wifi to the wifi with ssid and password from parameters and save it to the database
 * @param ssid      ssid of wifi
 * @param password  password of wifi
 */
wifi_result_t wifiNetworkManager::changeWifi(String ssid, String password)
{
    std::lock_guard<std::mutex> lock(_mutex__);

    WiFi.disconnect();
    if(WiFi.begin(ssid.c_str(), password.c_str())){
        _connectedWifiNetwork = ssid;
        std::vector<String> WifiConnectionDetails;
        WifiConnectionDetails.push_back(ssid);
        WifiConnectionDetails.push_back(password);
        _wifiDatabaze->addRecord(WifiConnectionDetails);

        return CONNECTED;
    }
    return NO_WIFI;
};

/**
 * Change wifi to the wifi with ssid from parameters and save it to the database
 * @param ssid      ssid of wifi
 * @return CONNECTED if wifi is connected, UNKNOW_WIFI if wifi is not in database, NO_WIFI if this wifi is not available
*/
wifi_result_t wifiNetworkManager::changeWifi(String ssid)
{    
    std::lock_guard<std::mutex> lock(_mutex__);

    std::vector<String> WifiConnectionDetails = _wifiDatabaze->getRecord(ssid.c_str());
    if(WifiConnectionDetails.size() > 1){
        String oldSsid = WiFi.SSID();
        String oldPassword = WiFi.psk();
        WiFi.disconnect();
        if(WiFi.begin(ssid.c_str(), WifiConnectionDetails[1].c_str()) == WL_CONNECTED){
            _connectedWifiNetwork = ssid;
            return CONNECTED;
        }
        else
        {
            WiFi.begin(oldSsid.c_str(), oldPassword.c_str());
            return NO_WIFI;
        }
    }
    else
    {
        return UNKNOW_WIFI;
    }
};

// private:  ********************************************************************************************************************   private:

/**
 * Get ssid of available wifi that is also in your database
 * @return String with available ssid, if there is no wifi in database, return ""
 */
String wifiNetworkManager::getAvailableWifiFromDatabaze()
{
    std::vector<String> wifisFromDatabaze = _wifiDatabaze->getRecordsName();
    wifisFromDatabaze.erase(wifisFromDatabaze.begin()); // delete first element, because it is header
    wifisFromDatabaze.erase(wifisFromDatabaze.begin()); // delete second element, because it is AP wifi
    std::vector<wifiNetwork> availableWifiList = getAvailableWifiList();
    if((wifisFromDatabaze.size() == 0)||(availableWifiList.size() == 0)){
        printf("Error: no WiFi available\n");
        return "";
    }
    for(int i = 0; i < wifisFromDatabaze.size(); i++){
        for(int j = 0; j < availableWifiList.size(); j++){
            if(wifisFromDatabaze.at(i) == availableWifiList.at(j).ssid){
                return wifisFromDatabaze.at(i);
            }
        }
    }
    printf("Error: no WiFi available\n");
    return "";
};

/**
 * Run the AP with ssid and password from the database and if the AP is not in the database, start AP with default ssid and password from parameters
 * @param AP_ssid       ssid of AP
 * @param AP_password   password of AP
 */
void wifiNetworkManager::startAP(const char* AP_ssid, const char* AP_password)
{
    std::vector<String> ssidName = _wifiDatabaze->getRecord(1);
    if(ssidName.size() > 0){
        AP_ssid = ssidName[0].c_str();
        AP_password = ssidName[1].c_str();
    }

    WiFi.softAP(AP_ssid, AP_password);
};