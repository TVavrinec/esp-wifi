#include "csvDatabese.hpp"
#include "pageManager.hpp"
#include "wifiNetworkManager.hpp"

#include "tunnel.hpp"


/* Put IP Address details */
// int progress_bar_value = 0;
IPAddress local_IP(192,168,0,171);
IPAddress gateway(192, 168,0,99);
IPAddress subnet(255, 255, 0, 0);

IPAddress AP_IP(192,168,4,1);
IPAddress AP_gateway(192, 168,4,2);


IPAddress clientId(192,168,0,172);


csvDatabese userList("/users.csv");
csvDatabese wifiList("/wifi_network.csv");

// ************************************************************************************************************************************************************************************
#define FORMAT_LITTLEFS_IF_FAILED true


void setup(){
  Serial0.begin(115200);
  printf("\nprint user list\n");
  userList.printDatabase();
  printf("\n print wifi list\n");
  wifiList.printDatabase();
  printf("\n");

  if(!WiFi.config(local_IP, gateway, subnet)) {
    printf("STA Failed to configure\n");
  }
  
  wifiNetworkManager wifiManager(&wifiList);

  TunnelTcpUart tunnel(1234, clientId, Serial0);
  tunnel.begin();

  while (true)
  {
    delay(1000);
  }
  

  std::vector<csvDatabese *> csvDatabeses;
  csvDatabeses.push_back(&wifiList);
  csvDatabeses.push_back(&userList);
  pageManager page(&wifiManager, &userList, &csvDatabeses);

  while(1){
    delay(10000);
  }
}

void loop(){
  delay(1000);
}
