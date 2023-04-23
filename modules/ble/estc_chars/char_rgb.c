#include <app_timer.h>
#include "char_rgb.h"
#include "modules/ble/gatt/estc_gatt_srv_char.h"
#include "modules/hsv/hsv_picker.h"

#define ESTC_CHAR_RGB_NOTIF_DELAY APP_TIMER_TICKS(2500)

APP_TIMER_DEF(m_char_rgb_notif);

// Description in UTF-8: "LED values in RGB format. The first octet-red LED, the subsequent octet-green LED, the last octet-blue LED."
static uint8_t rgb_desc[] = { 0x4c, 0x45, 0x44, 0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x73, 0x20, 0x69, 0x6e, 0x20, 
                              0x52, 0x47, 0x42, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x2e, 0x20, 0x54, 0x68, 
                              0x65, 0x20, 0x66, 0x69, 0x72, 0x73, 0x74, 0x20, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 
                              0x72, 0x65, 0x64, 0x20, 0x4c, 0x45, 0x44, 0x2c, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 
                              0x75, 0x62, 0x73, 0x65, 0x71, 0x75, 0x65, 0x6e, 0x74, 0x20, 0x6f, 0x63, 0x74, 0x65, 
                              0x74, 0x2d, 0x67, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x4c, 0x45, 0x44, 0x2c, 0x20, 0x74, 
                              0x68, 0x65, 0x20, 0x6c, 0x61, 0x73, 0x74, 0x20, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 
                              0x62, 0x6c, 0x75, 0x65, 0x20, 0x4c, 0x45, 0x44, 0x2e};

static uint8_t rgb_value[3];

static estc_ble_srv_char_handles_t rgb_ble_handles;

// static rgb_value_t estc_char_rgb_convert_char(void)
// {
//     return (rgb_value_t) {
//         .red = (uint16_t)rgb_value[0],
//         .green = (uint16_t)rgb_value[1],
//         .blue = (uint16_t)rgb_value[2],
//     };
// }

static void estc_char_convert_to_char(rgb_value_t * rgb, uint8_t rgb_char[3])
{
    rgb_char[0] = rgb->red;
    rgb_char[1] = rgb->green;
    rgb_char[2] = rgb->blue;
}

static void estc_char_rgb_notif_handler(void * ctx)
{
    rgb_value_t rgb;
    hsv_picker_get_current_rgb(&rgb);
    estc_char_convert_to_char(&rgb, rgb_value);
    uint16_t conn_handle = (uint16_t)((uintptr_t)ctx);
    uint16_t value_len = (uint16_t)(sizeof(rgb_value));
    estc_ble_srv_char_notify(conn_handle, rgb_ble_handles.value_handle.service_handle, rgb_value, &value_len);
}

ret_code_t estc_char_rgb_register(estc_ble_service_t * service, rgb_value_t * rgb)
{
    if (service == NULL || rgb == NULL)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    ret_code_t err_code = NRF_SUCCESS;

    err_code = app_timer_create(&m_char_rgb_notif, APP_TIMER_MODE_REPEATED, estc_char_rgb_notif_handler);

    estc_char_convert_to_char(rgb, rgb_value);

    estc_ble_srv_char_cfg_t char_config = 
    {
        .characteristic_uuid = ESTC_CHAR_RGB_UUID16,
        .characteristic_uuid_type = BLE_UUID_TYPE_UNKNOWN,
        .description_string = rgb_desc,
        .description_size = sizeof(rgb_desc),
        .is_value_on_stack = false,
        .permissions = ESTC_BLE_CHAR_READ | ESTC_BLE_CHAR_NOTIFICATION,
        .value = rgb_value,
        .value_size = sizeof(rgb_value),
        .value_size_max = sizeof(rgb_value),
        .value_type = ESTC_BLE_CHAR_TYPE_INT64,
    };

    err_code = estc_ble_srv_char_register(service, &char_config, &rgb_ble_handles);

    return err_code;
}

void estc_char_rgb_start_notifing(uint16_t conn_handle)
{
    app_timer_start(m_char_rgb_notif, ESTC_CHAR_RGB_NOTIF_DELAY, (void *)((uintptr_t)conn_handle));
}

void estc_char_rgb_stop_notifing(uint16_t conn_handle)
{
    app_timer_stop(m_char_rgb_notif);
}
