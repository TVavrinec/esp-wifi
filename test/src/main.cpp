#include <WiFi.h>

// debug log, set to 1 to enable
#define ENABLE_DEBUG_LOG 1

// wifi config
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASS";

// ethernet config
const IPAddress local_IP(192, 168, 0, 98);
const IPAddress gateway(192, 168, 0, 99);
const IPAddress subnet(255, 255, 255, 0);
const IPAddress primaryDNS(8, 8, 8, 8);
const IPAddress secondaryDNS(8, 8, 4, 4);

// rs-server config
const int serverPort = 1234;

// rs port config
const int baudrate = 115200;
const int rs_config = SERIAL_8N1;

// reading buffor config
#define BUFFER_SIZE 1024

// global objects
WiFiServer server;
byte buff[BUFFER_SIZE];

void setup()
{
    // init rs port
    Serial.begin(115200); // baudrate, rs_config

    // init wifi connection
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    {
        printf("log: Failed to configure network settings\n");
    }

    WiFi.begin("Vodafone-AEA0", "WS677xkrpuh2arAe");
    WiFi.softAP("my_WiFi", "12345678");

    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     printf("log: connecting to WiFi network\n");
    //     delay(500);
    // }

#if ENABLE_DEBUG_LOG == 1
    Serial.println("connected to WiFi");
    Serial.print("IP adddr: ");
    Serial.println(WiFi.localIP());
#endif
    delay(1000);

    // start server
    server = WiFiServer(serverPort);
    server.begin();
    delay(1000);
    printf("log: server started\n");
}

void loop()
{
    // wait for client
    WiFiClient client = server.available();
    if (!client)
        return;

    printf("log: client found\n");
    while (client.connected())
    {
        int size = 0;

        // read data from wifi client and send to serial
        while ((size = client.available()))
        {
            size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
            client.read(buff, size);
            Serial.write(buff, size);
            Serial.flush();
        }

        // read data from serial and send to wifi client
        while ((size = Serial.available()))
        {
            size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
            Serial.readBytes(buff, size);
            client.write(buff, size);
            client.flush();
        }
    }
    printf("log: client disconnected\n");
    client.stop();
}