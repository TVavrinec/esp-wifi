#pragma once
#include <WiFi.h>

#define BUFFER_SIZE 1024

class TunnelTcpUart
{
private:
    int _uartPort;
    int _uartBoundRate;
    int _tcpPort;
    uint8_t _buff[BUFFER_SIZE];

    TaskHandle_t _tunnel = NULL;

    void loop(WiFiClient *clientWifiTunnel, HardwareSerial *localUart);
    static void tunnelProcess(void *self_);

public:
    TunnelTcpUart(int tcpPort, int uartPort, int baudRate = 115200);
    ~TunnelTcpUart();

    void start();
    void end();
};


class TunnelTcpTcp
{
private:
    int _tcpPort0;
    int _tcpPort1;
    uint8_t _buff[BUFFER_SIZE];

    TaskHandle_t _tunnel = NULL;

    void loop(WiFiClient *clientWifiTunnel, HardwareSerial *localUart);
    static void tunnelProcess(void *self_);

public:
    TunnelTcpTcp(int tcpPort0, int tcpPort1);

    void start();
    void end();
};
