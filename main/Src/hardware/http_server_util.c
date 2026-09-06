#include "hardware/http_server_util.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif_types.h"
#include "esp_wifi_types_generic.h"
#include "http_server_handlers_conf.h"

static const char *TAG = "HTTP_SERVER_UTIL";

static httpd_handle_t HTTPD_SERVER_HANDLER = NULL;

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
		UAL_HTTP_SERVER_HANDLERS_CONF_Init(server);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    return httpd_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data) {
	httpd_handle_t *server = (httpd_handle_t *)arg;
	if (*server) {
		ESP_LOGI(TAG, "Stopping webserver");
		if (stop_webserver(*server) == ESP_OK) {
			*server = NULL;
		} else {
			ESP_LOGE(TAG, "Failed to stop http server");
		}
	}
}

static void connect_handler(void *arg, esp_event_base_t event_base,
							int32_t event_id, void *event_data) {
	httpd_handle_t *server = (httpd_handle_t *)arg;
	if (*server == NULL) {
		ESP_LOGI(TAG, "Starting webserver");
		*server = start_webserver();
	}
}

void UAL_HTTP_SERVER_UTIL_Init() {
	ESP_ERROR_CHECK(esp_event_handler_register(
		IP_EVENT, 
		IP_EVENT_STA_GOT_IP,
		&connect_handler,
		&HTTPD_SERVER_HANDLER
	));
	
	ESP_ERROR_CHECK(esp_event_handler_register(
		WIFI_EVENT, 
		WIFI_EVENT_STA_DISCONNECTED,
		&disconnect_handler, 
		&HTTPD_SERVER_HANDLER
	));
	
	HTTPD_SERVER_HANDLER = start_webserver();
}
