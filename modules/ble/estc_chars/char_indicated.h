#ifndef ESTC_CHAR_INDICATED_H_
#define ESTC_CHAR_INDICATED_H_

#include <app_error.h>
#include <app_timer.h>
#include "modules/ble/gatt/estc_gatt_srv.h"

#define ESTC_INDICATED_CHAR_UUID16 0x06ba
#define ESTC_INDICATED_VALUE_DELAY APP_TIMER_TICKS(3500)

ret_code_t estc_char_indicated_register(estc_ble_service_t * service);

void estc_char_indicated_start(uint16_t conn_handle);

void estc_char_indicated_stop(uint16_t conn_handle);

#endif // ESTC_CHAR_INDICATED_H_