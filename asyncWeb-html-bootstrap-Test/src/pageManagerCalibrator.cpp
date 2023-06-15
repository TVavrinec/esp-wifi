#include "pageManager.hpp"

void pageManager::calibratorProcess(cJSON& json)
{

}

void pageManager::pushCalibratorPage()
{
    serveStaticFiles("/users/calibrator/");
    serveStaticFiles("/web/");
    _ws->textAll("{\"type\":\"login\",\"user\":\"calibrator\"}");
}