#include <string.h>
#include "web_server.h"
#include "esp_log.h"

static esp_err_t http_get_handler(httpd_req_t *req);

httpd_uri_t uri_get = {
    .uri       = "/*",
    .method    = HTTP_GET,
    .handler   = http_get_handler,
    .user_ctx  = NULL
};

static esp_err_t http_get_handler(httpd_req_t *req)
{
    const char *uri = req->uri;
    char path[128];
    snprintf(path, sizeof(path) - 1, "/littlefs%.*s", (int)(sizeof(path) - 10 - 1), uri);
    
    FILE* file = fopen(path, "r");
    if (!file) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    
    char content_type[32];
    if (strcmp(uri, "/") == 0 || strstr(uri, ".html")) {
        strcpy(content_type, "text/html");
    } else if (strstr(uri, ".css")) {
        strcpy(content_type, "text/css");
    } else if (strstr(uri, ".js")) {
        strcpy(content_type, "application/javascript");
    } else {
        strcpy(content_type, "text/plain");
    }

    httpd_resp_set_type(req, content_type);

    char buf[1024];
    ssize_t read_len;
    while ((read_len = fread(buf, 1, sizeof(buf), file)) > 0) {
        httpd_resp_send_chunk(req, buf, read_len);
    }

    fclose(file);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

httpd_handle_t start_web_server(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI("WEBSERVER", "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
    }

    return server;
}
