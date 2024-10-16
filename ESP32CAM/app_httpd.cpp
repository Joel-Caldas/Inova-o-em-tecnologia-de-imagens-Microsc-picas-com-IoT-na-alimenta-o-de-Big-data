#include "esp_http_server.h"
#include "esp_log.h"
#include "camera_index.h"
// #include "camera_control.h" // Removido porque não é necessário
#include "sdkconfig.h"
#include "esp_camera.h"

static const char *TAG = "camera_httpd";

httpd_handle_t camera_httpd = NULL;

/* URI handlers */
extern "C" {
    extern esp_err_t index_handler(httpd_req_t *req);
    extern esp_err_t status_handler(httpd_req_t *req);
    extern esp_err_t cmd_handler(httpd_req_t *req);
    extern esp_err_t capture_handler(httpd_req_t *req);
    extern esp_err_t bmp_handler(httpd_req_t *req);
    extern esp_err_t xclk_handler(httpd_req_t *req);
    extern esp_err_t reg_handler(httpd_req_t *req);
    extern esp_err_t greg_handler(httpd_req_t *req);
    extern esp_err_t pll_handler(httpd_req_t *req);
    extern esp_err_t win_handler(httpd_req_t *req);
}

/* Start the web server for handling the camera and control requests */
