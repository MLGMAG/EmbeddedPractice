#include "http_server_handlers_conf.h"
#include "service/http/sensors_http_handler.h"

static const httpd_uri_t URI_SENSORS_GET = {
	.uri       = "/api/v1/sensors",
    .method    = HTTP_GET,
    .handler   = UAL_SENSORS_HTTP_HANDLER_Handle
};

void UAL_HTTP_SERVER_HANDLERS_CONF_Init(httpd_handle_t server) {
	httpd_register_uri_handler(server, &URI_SENSORS_GET);
}
