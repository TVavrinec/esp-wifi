#include "pageManager.hpp"

user_permission pageManager::calibratorProcess(cJSON& json)
{

    return CALIBRATOR;
}

void pageManager::pushCalibratorPage()
{
    serveStaticFiles("/users/calibrator/");
    serveStaticFiles("/web/");
    _ws->textAll("{\"type\":\"login\",\"user\":\"calibrator\"}");
}