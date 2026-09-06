#ifndef __SENSORS_HTTP_HANDLER_H
#define __SENSORS_HTTP_HANDLER_H

#include "esp_http_server.h"

esp_err_t UAL_SENSORS_HTTP_HANDLER_Handle(httpd_req_t *req);

#endif /* __SENSORS_HTTP_HANDLER_H */