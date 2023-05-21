
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tunnel.hpp"

Tunnel::Tunnel(int tcp_port, int uart_port, int baud_rate)
{
    if (uart_port > 2 || uart_port < 0)
    {
        printf("ERROR: uart port must be 0, 1 or 2\n");
        return;
    }
    uartPort = uart_port;
    uartBoundRate = baud_rate;
    tcpPort = tcp_port;
}

void Tunnel::loop(WiFiClient *clientWifiTunnel, HardwareSerial *local_uart){
    int size = 0;
    

    // read data from wifi client and send to serial
    while ((size = clientWifiTunnel->available()))
    {
        size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
        clientWifiTunnel->read(buff, size);
        local_uart->write(buff, size);
        local_uart->flush();
    }

    // read data from serial and send to wifi client
    while ((size = local_uart->available()))
    {
        size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
        local_uart->readBytes(buff, size);
        clientWifiTunnel->write(buff, size);
        clientWifiTunnel->flush();
    }
}

void Tunnel::tunnelProcess(void *self_)
{
    auto *self = (Tunnel*)self_;

    HardwareSerial uartSerial(self->uartPort);
    uartSerial.begin(self->uartBoundRate);
    uartSerial.println("uartSerial");

    WiFiServer wifiServer(self->tcpPort);
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

void Tunnel::start()
{
    TaskHandle_t xHandle = NULL;
    xTaskCreate(tunnelProcess , "tunnelProcess", 4096, this, 1, &xHandle);
}

void Tunnel::end() 
{
}
