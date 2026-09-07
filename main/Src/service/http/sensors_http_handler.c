#include "service/http/sensors_http_handler.h"
#include "service/sensors_data_storage.h"
#include "util/json_util.h"

static char http_buffer[256];

esp_err_t UAL_SENSORS_HTTP_HANDLER_Handle(httpd_req_t *req) {
	SENSORS_DATA_t data = UAL_SENSORS_DATA_STORAGE_Get();
	UAL_JSON_UTIL_ConvertSensorsDataToJson(http_buffer, &data);

	httpd_resp_set_hdr(req, "Content-Type", "application/json");
	httpd_resp_send(req, http_buffer, HTTPD_RESP_USE_STRLEN);
	
	return ESP_OK;
}
