
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tunnel.hpp"

TunnelTcpUart::TunnelTcpUart(int tcpPort, int uartPort, int baudRate)
{
    if (uartPort > 2 || uartPort < 0)
    {
        printf("ERROR: uart port must be 0, 1 or 2\n");
        return;
    }
    _uartPort = uartPort;
    _uartBoundRate = baudRate;
    _tcpPort = tcpPort;
}

TunnelTcpUart::~TunnelTcpUart()
{
    end();
}

void TunnelTcpUart::loop(WiFiClient *clientWifiTunnel, HardwareSerial *local_uart){
    int size = 0;
    

    // read data from wifi client and send to serial
    while ((size = clientWifiTunnel->available()))
    {
        size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
        clientWifiTunnel->read(_buff, size);
        local_uart->write(_buff, size);
        local_uart->flush();
    }

    // read data from serial and send to wifi client
    while ((size = local_uart->available()))
    {
        size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
        local_uart->readBytes(_buff, size);
        clientWifiTunnel->write(_buff, size);
        clientWifiTunnel->flush();
    }
}

void TunnelTcpUart::tunnelProcess(void *self_)
{
    auto *self = (TunnelTcpUart*)self_;

    HardwareSerial uartSerial(self->_uartPort);
    uartSerial.begin(self->_uartBoundRate);
    uartSerial.println("uartSerial");

    WiFiServer wifiServer(self->_tcpPort);
    wifiServer.begin();
    
    while (true){
        WiFiClient clientWifiTunnel = wifiServer.available();        
        while (clientWifiTunnel.connected())
        {
            self->loop(&clientWifiTunnel, &uartSerial);
        }
    }
    uartSerial.end();
}

void TunnelTcpUart::start()
{
    TaskHandle_t xHandle = NULL;
    xTaskCreate(tunnelProcess , "tunnelProcess", 4096, this, 1, &xHandle);
}

void TunnelTcpUart::end() 
{
    vTaskDelete(_tunnel);
}




// TunnelTcpTcp::TunnelTcpTcp(int tcpPort0, int tcpPort1)
// {
//     _tcpPort0 = tcpPort0;
//     _tcpPort1 = tcpPort1;
// }

// TunnelTcpTcp::~TunnelTcpTcp()
// {
//     end();
// }

// void TunnelTcpTcp::loop(WiFiClient *clientWifiTunnel, HardwareSerial *local_uart){
//     int size = 0;
    

//     // read data from wifi client and send to serial
//     while ((size = clientWifiTunnel->available()))
//     {
//         size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
//         clientWifiTunnel->read(_buff, size);
//         local_uart->write(_buff, size);
//         local_uart->flush();
//     }

//     // read data from serial and send to wifi client
//     while ((size = local_uart->available()))
//     {
//         size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
//         local_uart->readBytes(_buff, size);
//         clientWifiTunnel->write(_buff, size);
//         clientWifiTunnel->flush();
//     }
// }

// void TunnelTcpTcp::tunnelProcess(void *self_)
// {
//     auto *self = (TunnelTcpUart*)self_;

//     wifiServer.begin();
//     WiFiClient clientA; 
//     WiFiClient clientB;
    
//     while (true){
//         if (!clientA.connected()) {
//             clientA = server.available();
//         }

//         if (!clientB.connected()) {
//             clientB = server.available();
//         }

//         // Přenos dat ze zařízení A na zařízení B
//         if (clientA && clientA.connected()) {
//             if (clientA.available()) {
//             char data = clientA.read();
//             Serial.print("Přijato od zařízení A: ");
//             Serial.println(data);

//             // Odeslat data na zařízení B
//             }
//         }

//         // Přenos dat ze zařízení B na zařízení A
//         if (clientB && clientB.connected()) {
//             if (clientB.available()) {
//             char data = clientB.read();
//             Serial.print("Přijato od zařízení B: ");
//             Serial.println(data);

//             // Odeslat data na zařízení A
//             }
//         }
//     }
// }

// void TunnelTcpTcp::start()
// {
//     TaskHandle_t xHandle = NULL;
//     xTaskCreate(tunnelProcess , "tunnelProcess", 4096, this, 1, &xHandle);
// }

// void TunnelTcpTcp::end() 
// {
// }
