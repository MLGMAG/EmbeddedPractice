#include "wifi_sta_util.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_wifi_types_generic.h"
#include "nvs_flash.h"
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *TAG = "WIFI_STA_UTIL";

static EventGroupHandle_t wifi_event_group;

static int retry_num = 0;

static void event_handler(void *arg, esp_event_base_t event_base,
						  int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT &&
			   event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (retry_num < CONFIG_WIFI_CONNECTION_MAX_RETRY) {
			esp_wifi_connect();
			retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		} else {
			xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
		}
		ESP_LOGI(TAG, "connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		retry_num = 0;
		xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

void UAL_WIFI_STA_UTIL_Init() {
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		WIFI_EVENT,       
		ESP_EVENT_ANY_ID,     
		&event_handler,      
		NULL,            
		&instance_any_id
	));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		IP_EVENT,           
		IP_EVENT_STA_GOT_IP,             
		&event_handler,              
		NULL,                
		&instance_got_ip
	));
	wifi_config_t wifi_config = {
		.sta = {.ssid = CONFIG_WIFI_SSID, .password = CONFIG_WIFI_PASSWORD},
	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	EventBits_t bits = xEventGroupWaitBits(
		wifi_event_group,
		WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
		pdFALSE, 
		pdFALSE, 
		pdMS_TO_TICKS(5000)
	);

	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(
			TAG, 
			"connected to SSID: '%s'", 
			CONFIG_WIFI_SSID,
			CONFIG_WIFI_PASSWORD
		);
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(
			TAG, 
			"failed to connect to SSID: '%s'",
			CONFIG_WIFI_SSID, 
			CONFIG_WIFI_PASSWORD
		);
	} else {
		ESP_LOGW(TAG, "UNEXPECTED EVENT");
	}
}
