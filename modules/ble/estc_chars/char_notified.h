#ifndef ESTC_CHAR_NOTIFIED_H_
#define ESTC_CHAR_NOTIFIED_H_

#include <app_error.h>
#include <stdint.h>
#include <app_timer.h>
#include "modules/ble/gatt/estc_gatt_srv.h"

#define ESTC_NOTIFIED_CHAR_UUID16 0x06b9
#define ESTC_NOTIFIED_VALUE_DELAY APP_TIMER_TICKS(2500)

ret_code_t estc_char_notified_register(estc_ble_service_t * service);

void estc_char_notified_start(uint16_t conn_handle);

void estc_char_notified_stop(uint16_t conn_handle);

#endif // ESTC_CHAR_NOTIFIED_H_