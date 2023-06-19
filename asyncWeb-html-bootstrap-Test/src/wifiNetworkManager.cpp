#include "wifiNetworkManager.hpp"

wifiNetworkManager::wifiNetworkManager(csvDatabese *wifiDatabaze, bool AP_always_on, const char* AP_ssid, const char* AP_password)
{
    _wifiDatabaze = wifiDatabaze;
    String availableWifi = getAvailableWifiFromDatabaze();
    if(availableWifi != ""){
        _connectedWifiNetwork = availableWifi;
        _connectedWifiPassword = _wifiDatabaze->getRecordCell(availableWifi.c_str(), "password");
        WiFi.begin(availableWifi.c_str(), _connectedWifiPassword.c_str());
        printf("Ssid: %s password: %s\n",availableWifi.c_str(), _connectedWifiPassword.c_str());
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            printf("Connecting to WiFi...\n");
        }
        printf("Connected to WiFi on IP address: %s\n", WiFi.localIP().toString().c_str());
        delay(3);
    }
    else
    {
        AP_always_on = true;
    }
    if(AP_always_on){
        _startAP(AP_ssid, AP_password);
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
 * Change active wifi in the wifi database
 * @param ssid      ssid of wifi
 * @param password  password of wifi
 * 
 * @return true if wifi is changed, false if wifi is not changed
 */
wifi_save_status_t wifiNetworkManager::changeActiveWifi(String ssid, String password)
{
    for(int i = _firstStationRecord; i < _wifiDatabaze->getNumberOfRecords(); i++){
        if(_wifiDatabaze->getRecordCell(i, "activity") == "active"){
            _wifiDatabaze->changeRecord(i, "activity", "active");
            return SAVED;
        }
    }
    if(changeActiveWifi(ssid) == UNSAVED){
        if((password.length() >= 8) && (password.length() <= 63) && (ssid.length() >= 2) && (ssid.length() <= 31)){
            _wifiDatabaze->addRecord(std::vector<String>{ssid, password, "active"});
            return SAVED;
        }
        else{
            return UNSAVED;
        }
    }
};

/**
 * Change active wifi in the wifi database
 * @param ssid      ssid of wifi
 * 
 * @return true if wifi is changed, false if wifi is not changed
 */
wifi_save_status_t wifiNetworkManager::changeActiveWifi(String ssid){
    String password = _wifiDatabaze->getRecordCell(ssid.c_str(), "password");
    if(password == ""){
        return UNSAVED;
    }
    for(int i = _firstStationRecord; i < _wifiDatabaze->getNumberOfRecords(); i++){
        if(_wifiDatabaze->getRecordCell(i, "activity") == "active"){
            _wifiDatabaze->changeRecord(i, "activity", "inactive");
            break;
        }
    }
    for(int i = _firstStationRecord; i < _wifiDatabaze->getNumberOfRecords(); i++){
        if(_wifiDatabaze->getRecordCell(i, "ssid") == ssid){
            _wifiDatabaze->changeRecord(i, "activity", "active");
            return SAVED;
        }
    }
    return UNSAVED;
}

/**
 * Change AP in the wifi database
 * @param ssid      ssid of AP
 * @param password  password of AP
 * 
 * @return true if AP is changed, false if AP is not changed
 */
wifi_save_status_t wifiNetworkManager::changeAP(String ssid, String password){
    if((password.length() >= 8) && (password.length() <= 63) && (ssid.length() >= 2) && (ssid.length() <= 31)){
        _wifiDatabaze->changeRecord(1, std::vector<String>{ssid, password,  _wifiDatabaze->getRecordCell(1, "activity")});
        return SAVED;
    }
    else{
        return UNSAVED;
    }
}

/**
 * Start station
 */
void wifiNetworkManager::startStation(){
    WiFi.begin(_connectedWifiNetwork.c_str(), _connectedWifiPassword.c_str());
};

/**
 * Stop station
 */
void wifiNetworkManager::stopStation(){
    WiFi.mode(WIFI_OFF);
};

/**
 * Stop AP
 */
void wifiNetworkManager::stopAP(){
    _wifiDatabaze->changeRecord(1, "activity", "inactive");
};

/**
 * Start AP with ssid and password from object memory
 */
void wifiNetworkManager::startAP(){
    _wifiDatabaze->changeRecord(1, "activity", "active");
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
void wifiNetworkManager::_startAP(const char* AP_ssid, const char* AP_password)
{
    std::vector<String> ssidName = _wifiDatabaze->getRecord(1);
    if(ssidName.size() > 0){
        AP_ssid = ssidName[0].c_str();
        AP_password = ssidName[1].c_str();
    }

    WiFi.softAP(AP_ssid, AP_password);
};