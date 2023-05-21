#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_server.h"

static const char *TAG = "server";

/* Handler for the root path "/" */
esp_err_t root_handler(httpd_req_t *req)
{
    /* Set response content type */
    httpd_resp_set_type(req, "text/html");
    /* Send a simple response body */
    httpd_resp_send(req, "<html><body><h1>Hello from ESP32!</h1></body></html>", -1);
    return ESP_OK;
}

/* URI handler structure for the root path */
httpd_uri_t root_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_handler,
    .user_ctx  = NULL
};

/* Start the web server */
httpd_handle_t start_webserver(void)
{
    /* Set the server configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    /* Start the server */
    httpd_handle_t server_handle;
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server_handle, &config) == ESP_OK) {
        /* Set URI handlers */
        httpd_register_uri_handler(server_handle, &root_uri);
        return server_handle;
    }
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

/* Stop the web server */
void stop_webserver(httpd_handle_t server_handle)
{
    /* Stop the server */
    httpd_stop(server_handle);
}

/* Main application function */
void app_main()
{
    /* Start the web server */
    httpd_handle_t server = start_webserver();

    /* Wait for shutdown signal */
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    /* Stop the web server */
    stop_webserver(server);
}