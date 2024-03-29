#include <app_timer.h>
#include <nrf_log.h>
#include "char_rgb.h"
#include "modules/ble/gatt/estc_gatt_srv_char.h"
#include "modules/hsv/hsv_picker.h"
#include "modules/ble/estc_ble_write_mngr.h"
#include "modules/ble/gap/bond/estc_bond.h"

#define ESTC_CHAR_RGB_NOTIF_DELAY APP_TIMER_TICKS(2500)

APP_TIMER_DEF(m_char_rgb_notif);

// Description in UTF-8: 
// "LED values in RGB format. The first octet-red LED, the subsequent octet-green LED, the last octet-blue LED."
static uint8_t rgb_desc[] = { 0x4c, 0x45, 0x44, 0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x73, 0x20, 0x69, 0x6e, 0x20, 
                              0x52, 0x47, 0x42, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x2e, 0x20, 0x54, 0x68, 
                              0x65, 0x20, 0x66, 0x69, 0x72, 0x73, 0x74, 0x20, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 
                              0x72, 0x65, 0x64, 0x20, 0x4c, 0x45, 0x44, 0x2c, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 
                              0x75, 0x62, 0x73, 0x65, 0x71, 0x75, 0x65, 0x6e, 0x74, 0x20, 0x6f, 0x63, 0x74, 0x65, 
                              0x74, 0x2d, 0x67, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x4c, 0x45, 0x44, 0x2c, 0x20, 0x74, 
                              0x68, 0x65, 0x20, 0x6c, 0x61, 0x73, 0x74, 0x20, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 
                              0x62, 0x6c, 0x75, 0x65, 0x20, 0x4c, 0x45, 0x44, 0x2e};

// UTF-8: "Change RED led intensity"
static uint8_t red_write_desc[] = {0x43, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x20, 0x52, 0x45, 0x44, 0x20, 0x6c, 0x65, 
                                   0x64, 0x20, 0x69, 0x6e ,0x74 ,0x65, 0x6e, 0x73, 0x69, 0x74, 0x79};

// UTF-8: "Change GREEN led intensity"
static uint8_t green_write_desc[] = {0x43, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x20, 0x47, 0x52, 0x45, 0x45, 0x4e, 0x20, 
                                     0x6c, 0x65, 0x64, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x6e, 0x73, 0x69, 0x74, 0x79};

// UTF-8: "Change BLUE led intensity"
static uint8_t blue_write_desc[] = {0x43, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x20, 0x42, 0x4c, 0x55, 0x45, 0x20, 0x6c, 
                                    0x65, 0x64, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x6e, 0x73, 0x69, 0x74, 0x79};

static uint8_t rgb_value[3];

static estc_ble_srv_char_handles_t rgb_read_ble_handles;

static estc_ble_srv_char_handles_t rgb_write_red_ble_handles;
static estc_ble_srv_char_handles_t rgb_write_green_ble_handles;
static estc_ble_srv_char_handles_t rgb_write_blue_ble_handles;

static rgb_value_t estc_char_rgb_convert_char(void)
{
    return (rgb_value_t) {
        .red = (uint16_t)rgb_value[0],
        .green = (uint16_t)rgb_value[1],
        .blue = (uint16_t)rgb_value[2],
    };
}

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
    estc_ble_srv_char_notify(conn_handle, rgb_read_ble_handles.value_handle, rgb_value, &value_len);
}

static void estc_char_rgb_update_read(uint16_t conn_handle)
{
    ble_gatts_value_t new_value = 
    {
        .p_value = rgb_value,
        .len = sizeof(rgb_value),
        .offset = 0U,
    };
    sd_ble_gatts_value_set(conn_handle, rgb_read_ble_handles.value_handle, &new_value);
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

    estc_ble_srv_char_cfg_t char_read_config = 
    {
        .characteristic_uuid = ESTC_CHAR_READ_RGB_UUID16,
        .characteristic_uuid_type = BLE_UUID_TYPE_UNKNOWN,
        .description_string = rgb_desc,
        .description_size = sizeof(rgb_desc),
        .permissions = ESTC_BLE_CHAR_READ | ESTC_BLE_CHAR_NOTIFICATION,
        .available = ESTC_BLE_CHAR_AVAIL_ALWAYS,
        .value = rgb_value,
        .value_size = sizeof(rgb_value),
        .value_size_max = sizeof(rgb_value),
        .value_type = ESTC_BLE_CHAR_TYPE_INT64,
    };

    err_code = estc_ble_srv_char_register(service, &char_read_config, &rgb_read_ble_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    estc_ble_srv_char_cfg_t char_write_red_config = 
    {
        .characteristic_uuid = ESTC_CHAR_WRITE_RED_UUID16,
        .characteristic_uuid_type = BLE_UUID_TYPE_UNKNOWN,
        .description_string = red_write_desc,
        .description_size = sizeof(red_write_desc),
        .permissions = ESTC_BLE_CHAR_WRITE,
        .available = ESTC_BLE_CHAR_AVAIL_AFTER_PAIRING,
        .value = &(rgb_value[0]),
        .value_size = sizeof(rgb_value[0]),
        .value_size_max = sizeof(rgb_value[0]),
        .value_type = ESTC_BLE_CHAR_TYPE_UINT64,
    };
    
    err_code = estc_ble_srv_char_register(service, &char_write_red_config, &rgb_write_red_ble_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    estc_ble_write_mngr_subscribe(rgb_write_red_ble_handles.value_handle, estc_char_rgb_red_write_handler);
    
    estc_ble_srv_char_cfg_t char_write_green_config = 
    {
        .characteristic_uuid = ESTC_CHAR_WRITE_GREEN_UUID16,
        .characteristic_uuid_type = BLE_UUID_TYPE_UNKNOWN,
        .description_string = green_write_desc,
        .description_size = sizeof(green_write_desc),
        .permissions = ESTC_BLE_CHAR_WRITE,
        .available = ESTC_BLE_CHAR_AVAIL_AFTER_PAIRING,
        .value = &(rgb_value[1]),
        .value_size = sizeof(rgb_value[1]),
        .value_size_max = sizeof(rgb_value[1]),
        .value_type = ESTC_BLE_CHAR_TYPE_UINT64,
    };
    
    err_code = estc_ble_srv_char_register(service, &char_write_green_config, &rgb_write_green_ble_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    estc_ble_write_mngr_subscribe(rgb_write_green_ble_handles.value_handle, estc_char_rgb_green_write_handler);

    estc_ble_srv_char_cfg_t char_write_blue_config = 
    {
        .characteristic_uuid = ESTC_CHAR_WRITE_BLUE_UUID16,
        .characteristic_uuid_type = BLE_UUID_TYPE_UNKNOWN,
        .description_string = blue_write_desc,
        .description_size = sizeof(blue_write_desc),
        .permissions = ESTC_BLE_CHAR_WRITE,
        .available = ESTC_BLE_CHAR_AVAIL_AFTER_PAIRING,
        .value = &(rgb_value[2]),
        .value_size = sizeof(rgb_value[2]),
        .value_size_max = sizeof(rgb_value[2]),
        .value_type = ESTC_BLE_CHAR_TYPE_UINT64,
    };
    
    err_code = estc_ble_srv_char_register(service, &char_write_blue_config, &rgb_write_blue_ble_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    estc_ble_write_mngr_subscribe(rgb_write_blue_ble_handles.value_handle, estc_char_rgb_blue_write_handler);

    return NRF_SUCCESS;
}

void estc_char_rgb_start_notifing(uint16_t conn_handle)
{
    app_timer_start(m_char_rgb_notif, ESTC_CHAR_RGB_NOTIF_DELAY, (void *)((uintptr_t)conn_handle));
}

void estc_char_rgb_stop_notifing(uint16_t conn_handle)
{
    app_timer_stop(m_char_rgb_notif);
}

void estc_char_rgb_red_write_handler(uint16_t conn_handle, uint8_t * data, uint16_t offset, uint16_t len)
{
    if (!estc_ble_gap_is_conn_bonded(conn_handle))
    {
        return;
    }

    rgb_value[0] = data[0];
    
    rgb_value_t new_rgb_value = estc_char_rgb_convert_char();
    hsv_picker_set_rgb(new_rgb_value.red, new_rgb_value.green, new_rgb_value.blue);

    estc_char_rgb_update_read(conn_handle);
}

void estc_char_rgb_green_write_handler(uint16_t conn_handle, uint8_t * data, uint16_t offset, uint16_t len)
{
    if (!estc_ble_gap_is_conn_bonded(conn_handle))
    {
        return;
    }
    
    if (len > sizeof(rgb_value[1]))
    {
        return;
    }

    rgb_value[1] = data[0];
    
    rgb_value_t new_rgb_value = estc_char_rgb_convert_char();
    hsv_picker_set_rgb(new_rgb_value.red, new_rgb_value.green, new_rgb_value.blue);
    estc_char_rgb_update_read(conn_handle);
}

void estc_char_rgb_blue_write_handler(uint16_t conn_handle, uint8_t * data, uint16_t offset, uint16_t len)
{
    if (!estc_ble_gap_is_conn_bonded(conn_handle))
    {
        return;
    }

    if (len > sizeof(rgb_value[2]))
    {
        return;
    }

    rgb_value[2] = data[0];
    
    rgb_value_t new_rgb_value = estc_char_rgb_convert_char();
    hsv_picker_set_rgb(new_rgb_value.red, new_rgb_value.green, new_rgb_value.blue);
    estc_char_rgb_update_read(conn_handle);
}