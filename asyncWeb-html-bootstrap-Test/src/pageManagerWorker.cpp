#include "pageManager.hpp"

void pageManager::workerProcess(cJSON& json)
{

}

void pageManager::pushWorkerPage()
{
    serveStaticFiles("/users/worker/");
    serveStaticFiles("/web/");
    _ws->textAll("{\"type\":\"login\",\"user\":\"worker\"}");
}
