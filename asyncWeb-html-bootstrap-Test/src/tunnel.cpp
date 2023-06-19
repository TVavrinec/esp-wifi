
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tunnel.hpp"

//************************************************************************************************************************************************************************************
TunnelTcpUart::TunnelTcpUart(int tcpPort, IPAddress clientId, HardwareSerial &uart)
: _clientId(clientId), _uart(uart)
{
    _uart = uart;
    _server = new AsyncServer(tcpPort);
    _clientId = clientId;
    printf("TunnelTcpUart created\n");
};

TunnelTcpUart::~TunnelTcpUart()
{
    end();
    delete _server;
}

void TunnelTcpUart::begin()
{
    _server->begin();
    printf("TunnelTcpUart begin\n");
    _server->onClient([](void *h, AsyncClient* client){
        TunnelTcpUart *self = (TunnelTcpUart *)h;
        self->_clients.push_back(client);
        printf("self->_clientId: %s\n", self->_clientId.toString().c_str());
        printf("client->remoteIP: %s\n", client->remoteIP().toString().c_str());

        // Handle client disconnection
        client->onDisconnect([](void *h, AsyncClient* client){
            TunnelTcpUart *self = (TunnelTcpUart *)h;
            self->_clients.erase(std::remove(self->_clients.begin(), self->_clients.end(), client), self->_clients.end());
            delete client;
        }, self);

        // Handle incoming data
        client->onData([](void *obj, AsyncClient* c, void *data, size_t len){
            TunnelTcpUart *self = (TunnelTcpUart *)obj;
            if(c->remoteIP() == self->_clientId){
                self->_uart.write((uint8_t*)data, len);
            }
        }, self);
    }, this);

    xTaskCreate([](void *self_) {
            TunnelTcpUart *self = (TunnelTcpUart *)self_;
            while (true)
            {
                if (self->_uart.available())
                {
                    char data = self->_uart.read();
                    for (AsyncClient* client : self->_clients) {
                        if (client->remoteIP() == self->_clientId) {
                            client->write(&data, 1);
                        }
                    }
                }
                else
                    vTaskDelay(1/portTICK_PERIOD_MS);
                taskYIELD();
            }
        },
        "TunnelTcpUart",
        10000,
        this,
        1,
        _ttuTaskHandle
    );
}

void TunnelTcpUart::end()
{
    _server->end();
    vTaskDelete(_ttuTaskHandle);
}


//************************************************************************************************************************************************************************************

TunnelTcpTcp::TunnelTcpTcp(int tcpPort, IPAddress clientIdA, IPAddress clientIdB)
: _clientIdA(clientIdA), _clientIdB(clientIdB)
{
    _server = new AsyncServer(tcpPort);
    _clientIdA = clientIdA;
    _clientIdB = clientIdB;
    printf("TunnelTcpTcp created\n");
}

TunnelTcpTcp::~TunnelTcpTcp()
{
    end();
    delete _server;
}

void TunnelTcpTcp::begin()
{
    _server->begin();
    printf("TunnelTcpUart begin\n");
    _server->onClient([](void *h, AsyncClient* client){
        TunnelTcpTcp *self = (TunnelTcpTcp *)h;
        self->_clients.push_back(client);
        printf("client->remoteIP: %s\n", client->remoteIP().toString().c_str());

        // Handle client disconnection
        client->onDisconnect([](void *h, AsyncClient* client){
            TunnelTcpTcp *self = (TunnelTcpTcp *)h;
            self->_clients.erase(std::remove(self->_clients.begin(), self->_clients.end(), client), self->_clients.end());
            delete client;
        }, self);

        // Handle incoming data
        client->onData([](void *obj, AsyncClient* c, void *data, size_t len){
            TunnelTcpTcp *self = (TunnelTcpTcp *)obj;
            if(c->remoteIP() == self->_clientIdA){
                for (AsyncClient* client : self->_clients) {
                    if(client->remoteIP() == self->_clientIdB) {
                        client->write((char *)data, len);
                        Serial0.printf("A:  %s\n", data);    
                    }
                }
            }
            else if(c->remoteIP() == self->_clientIdB){
                for (AsyncClient* client : self->_clients) {
                    if(client->remoteIP() == self->_clientIdA) {
                        client->write((char *)data, len);
                        Serial0.printf("B:  %s\n", data);    
                    }
                }
            }
        }, self);
    }, this);
}

void TunnelTcpTcp::end()
{
    _server->end();
}
