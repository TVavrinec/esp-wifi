#include "csvDatabese.hpp"
#include "pageManager.hpp"
#include "wifiNetworkManager.hpp"

#include "tunnel.hpp"

/* Put IP Address details */
// int progress_bar_value = 0;
IPAddress local_IP(192, 168, 0, 171);
IPAddress gateway(192, 168, 0, 99);
IPAddress subnet(255, 255, 0, 0);

IPAddress AP_IP(192, 168, 4, 1);
IPAddress AP_gateway(192, 168, 4, 2);

IPAddress clientId(192, 168, 0, 172);

csvDatabese communicationType("/type_of_communication.csv");
csvDatabese userList("/users.csv");
csvDatabese wifiList("/wifi_network.csv");

// ************************************************************************************************************************************************************************************
#define FORMAT_LITTLEFS_IF_FAILED true

void setup()
{
  String typeOfCommunication = communicationType.getRecord(0)[0];
  printf("Type of communication: %s\n", typeOfCommunication.c_str());
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    printf("STA Failed to configure\n");
  }
  wifiNetworkManager wifiManager(&wifiList);
  delay(1);

  // **************************************************
  if (typeOfCommunication == "web")
  {
    printf("\n\033[1;32mStart web page\033[0m\n");
    printf("\nprint user list\n");
    userList.printDatabase();
    printf("\n print wifi list\n");
    wifiList.printDatabase();
    printf("\n");

    std::vector<csvDatabese *> csvDatabeses;
    csvDatabeses.push_back(&wifiList);
    csvDatabeses.push_back(&userList);
    pageManager page(&wifiManager, &userList, &csvDatabeses);
    while (true)
      delay(1000);
  }
  else if (typeOfCommunication == "simpleTcp")
  {
    printf("uart\n");
    delay(1);
    Serial0.begin(115200);
    TunnelTcpTcp tunnel(1234, IPAddress(192, 168, 0, 248), IPAddress(192, 168, 0, 73));
    // TunnelTcpUart tunnel(1234, clientId, Serial0);
    tunnel.begin();
    while (true)
      delay(1000);
  }
  else
  {
    printf("Wrong type of communication\n");
  }

  while (true)
  {
    delay(1000);
  }

  while (1)
  {
    delay(10000);
  }
}

void loop()
{
  delay(1000);
}
