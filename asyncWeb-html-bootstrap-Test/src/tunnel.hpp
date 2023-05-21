#pragma once
#include <WiFi.h>

#define BUFFER_SIZE 1024

class Tunnel
{
private:
    int uartPort;
    int uartBoundRate;
    int tcpPort;
    uint8_t buff[BUFFER_SIZE];

    void loop(WiFiClient *clientWifiTunnel, HardwareSerial *localUart);
    static void tunnelProcess(void *self_);

public:
    Tunnel(int tcpPort, int uartPort, int baud_rate = 115200);

    void start();
    void end();
};
