#include "modules/ble/gatt/estc_gatt_srv_char.h"
#include "char_with_desc.h"

static uint8_t some_characteristic_desc[] = {0x73, 0x6f, 0x6d, 0x65, 0x20, 0x64, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67};

static uint32_t some_characteristic_value = 32U;

ret_code_t estc_char_with_desc_register(estc_ble_service_t * service)
{
    ret_code_t err_code = NRF_SUCCESS;

    estc_ble_srv_char_cfg_t basic_config =
    {
        .characteristic_uuid = ESTC_SOME_CHARACTERISTIC_UUID16,
        .description_string = some_characteristic_desc,
        .description_size = sizeof(some_characteristic_desc),
        .permissions = ESTC_BLE_CHAR_READ | ESTC_BLE_CHAR_WRITE,
        .value = (uint8_t *)(&some_characteristic_value),
        .value_size = sizeof(some_characteristic_value),
        .value_type = ESTC_BLE_CHAR_TYPE_UINT32,
        .value_size_max = sizeof(some_characteristic_value),
        .characteristic_uuid_type = BLE_UUID_TYPE_BLE,
    };

    estc_ble_srv_char_handles_t basic_handles;
    
    err_code = estc_ble_srv_char_register(service, &basic_config, &basic_handles);
    return err_code;
}
