#include "pageManager.hpp"

user_permission pageManager::workerProcess(cJSON& json)
{

    return WORKER;
}

void pageManager::pushWorkerPage()
{
    serveStaticFiles("/users/worker/");
    serveStaticFiles("/web/");
    _ws->textAll("{\"type\":\"login\",\"user\":\"worker\"}");
}

