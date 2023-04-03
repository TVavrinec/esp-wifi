#include <Arduino.h>

#include "BluetoothSerial.h" 

#include "gridui/src/gridui.h"
#include "rbprotocol.h"
#include "rbwebserver.h"
#include "rbwifi.h"




// ----------------------------------------------------------------
// void WiFi::connectAndStartAp(const char* sta_ssid, const char* sta_password, const char* ap_ssid, const char* ap_password, uint8_t ap_channel){
//     init();

//     esp_wifi_stop();

//     if (gNetIf) {
//         esp_netif_destroy(gNetIf);
//     }
//     if (gNetIfAP) {
//         esp_netif_destroy(gNetIfAP);
//     }

//     gNetIfAP = esp_netif_create_default_wifi_ap();
//     gNetIf   = esp_netif_create_default_wifi_sta();

//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

//     wifi_config_t cfg = {};

//     if (strlen(ap_password) >= 8) {
//         snprintf((char*)cfg.ap.password, 64, "%s", ap_password);
//         cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
//     } else {
//         ESP_LOGE(TAG, "The WiFi password is too short, 8 characters required, leaving the WiFI open!");
//         cfg.ap.authmode = WIFI_AUTH_OPEN;
//     }
//     snprintf((char*)cfg.ap.ssid, 32, "%s", ap_ssid);
//     cfg.ap.channel = ap_channel;
//     cfg.ap.beacon_interval = 400;
//     cfg.ap.max_connection = 4;
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &cfg));

//     cfg = {};

//     snprintf((char*)cfg.sta.ssid, 32, "%s", sta_ssid);
//     snprintf((char*)cfg.sta.password, 64, "%s", sta_password);
//     esp_wifi_set_config(WIFI_IF_STA, &cfg);

//     ESP_ERROR_CHECK(esp_wifi_start());

//     // esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);
// }




using namespace rb;
using namespace gridui;

// You can include layout.hpp in many .cpp files,
// but ONE of those must have this define before it.
#define GRIDUI_LAYOUT_DEFINITION
#include "layout.hpp"

// #include "SmartLeds.h"

static Protocol* gProt = nullptr;

static void onPacketReceived(const std::string& cmd, rbjson::Object* pkt) {
    // Let GridUI handle its packets
    if (UI.handleRbPacket(cmd, pkt))
        return;

    // ...any other non-GridUI packets
}

// static SmartLed gRgbLed(LED_WS2812, 1, 26);
BluetoothSerial ESP_BT;

void setup() {
    // gRgbLed[0] = Rgb(255, 0, 0);
    // gRgbLed.show();

    //  ------------------------------------------------------- //WiFi::connect("Anthrophobia", "Ku1ata2elvA");   // 192.168.0.171/index.html
    // printf("Connecting to ESP_BT\n");
    // ESP_BT.begin("ESP32_Control");

    // delay(10000);

    // printf("Connected to ESP_WiFi\n");
    // ESP_BT.disconnect();
    // ESP_BT.end();
    // delay(100);
    // WiFi::connect("Vodafone-AEA0", "WS677xkrpuh2arAe");
    WiFi::startAp("my_WiFi", "12345678");
    // WiFi::connectAndStartAp("Vodafone-AEA0", "WS677xkrpuh2arAe", "my_WiFi", "12345678");

    

    // Initialize RBProtocol
    gProt = new Protocol("FrantaFlinta", "Robocop", "Compiled at " __DATE__ " " __TIME__, onPacketReceived);
    gProt->start();

    // Start serving the web page
    rb_web_start(80);

    // Initialize the UI builder
    UI.begin(gProt);

    // Build the UI widgets. Positions/props are set in the layout, so most of the time,
    // you should only set the event handlers here.
    auto builder = Layout.begin();

    builder.Button1.onPress([](Button& b) {
        printf("Button 1 pressed\n");
    });


    // builder.sliderG.onChanged([](Slider& s) {
    //     printf("Slider G: %d)\n", s.value());
    //     // gRgbLed[0].g = s.value();
    //     // gRgbLed.wait();
    //     // gRgbLed.show();
    // });

    // Commit the layout. Beyond this point, calling any builder methods on the UI is invalid.
    builder.commit();
}

void loop() {
    printf("IP: %x\n", WiFi::getIp());
    delay(1000);
}
