#ifndef ESTC_CHAR_RGB_H_
#define ESTC_CHAR_RGB_H_

#include <stdint.h>
#include <app_error.h>
#include "modules/ble/gatt/estc_gatt_srv.h"
#include "modules/hsv/hsv_converter.h"

#define ESTC_CHAR_READ_RGB_UUID16 0x069B

#define ESTC_CHAR_WRITE_RED_UUID16 0x069C
#define ESTC_CHAR_WRITE_GREEN_UUID16 0x069D
#define ESTC_CHAR_WRITE_BLUE_UUID16 0x069E

ret_code_t estc_char_rgb_register(estc_ble_service_t * service, rgb_value_t * rgb);

void estc_char_rgb_start_notifing(uint16_t conn_handle);

void estc_char_rgb_stop_notifing(uint16_t conn_handle);

void estc_char_rgb_red_write_handler(uint16_t conn_handle, uint8_t * data, uint16_t offset, uint16_t len);

void estc_char_rgb_green_write_handler(uint16_t conn_handle, uint8_t * data, uint16_t offset, uint16_t len);

void estc_char_rgb_blue_write_handler(uint16_t conn_handle, uint8_t * data, uint16_t offset, uint16_t len);

#endif // ESTC_CHAR_RGB_H_