#ifndef ESTC_CHAR_RGB_H_
#define ESTC_CHAR_RGB_H_

#include <stdint.h>
#include <app_error.h>
#include "modules/ble/gatt/estc_gatt_srv.h"
#include "modules/hsv/hsv_converter.h"

#define ESTC_CHAR_RGB_UUID16 0x069B

ret_code_t estc_char_rgb_register(estc_ble_service_t * service, rgb_value_t * rgb);

void estc_char_rgb_start_notifing(uint16_t conn_handle);

void estc_char_rgb_stop_notifing(uint16_t conn_handle);

#endif // ESTC_CHAR_RGB_H_