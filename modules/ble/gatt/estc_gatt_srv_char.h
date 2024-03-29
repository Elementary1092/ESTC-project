#ifndef ESTC_BLE_SERVICE_CHARACTERISTIC_H_
#define ESTC_BLE_SERVICE_CHARACTERISTIC_H_

#include "estc_gatt_srv.h"

/**
 * @brief Possible characteristic property values.
 */
typedef enum
{
	ESTC_BLE_CHAR_BROADCAST = 1 << 0,
	ESTC_BLE_CHAR_READ = 1 << 1,
	ESTC_BLE_CHAR_WRITE_NO_RESP = 1 << 2,
	ESTC_BLE_CHAR_WRITE = 1 << 3,
	ESTC_BLE_CHAR_NOTIFICATION = 1 << 4,
	ESTC_BLE_CHAR_INDICATION = 1 << 5,
	ESTC_BLE_CHAR_AUTH_WRITE = 1 << 6,
	ESTC_BLE_CHAR_RELIABLE_WRITE = 1 << 7,
	ESTC_BLE_CHAR_WRITABLE_AUX = 1 << 8
} estc_ble_gatt_srv_char_allowed_ops_t;

typedef enum
{
	ESTC_BLE_CHAR_TYPE_UINT8,
	ESTC_BLE_CHAR_TYPE_UINT16,
	ESTC_BLE_CHAR_TYPE_UINT32,
	ESTC_BLE_CHAR_TYPE_UINT64,
	ESTC_BLE_CHAR_TYPE_INT8,
	ESTC_BLE_CHAR_TYPE_INT16,
	ESTC_BLE_CHAR_TYPE_INT32,
	ESTC_BLE_CHAR_TYPE_INT64,
	ESTC_BLE_CHAR_TYPE_FLOAT32,
	ESTC_BLE_CHAR_TYPE_FLOAT64,
	ESTC_BLE_CHAR_TYPE_STRING,
} estc_ble_srv_char_value_type_t;

typedef enum 
{
	ESTC_BLE_CHAR_AVAIL_ALWAYS,
	ESTC_BLE_CHAR_AVAIL_AFTER_PAIRING,
} estc_ble_char_avail_t;

typedef struct
{
	estc_ble_srv_char_value_type_t value_type; /**< Type of the value of a characteristic. */
	estc_ble_char_avail_t          available;  /**< Availability of the characteristic to the GATT client. */
	uint16_t permissions;					   /**< Should be composed of @ref estc_ble_gatt_srv_char_allowed_ops_t. */
	uint16_t description_size;				   /**< If description is present, should be != 0. */
	uint16_t value_size;					   /**< Size of a value in Bytes. */
	uint16_t characteristic_uuid;			   /**< 16 bit characteristic id */
	uint16_t value_size_max;				   /**< If value size is variable should be != 0 and != value_size */
	uint8_t characteristic_uuid_type;		   /**< Will be filled by @ref estc_ble_srv_char_register */
	uint8_t *value;							   /**< Pointer to a characteristic value*/
	uint8_t const *description_string;		   /**< UTF-8 string */
} estc_ble_srv_char_cfg_t;

typedef struct
{
	uint16_t value_handle;
	uint16_t user_description_handle;
	uint16_t client_char_desc_handle;
	uint16_t server_char_desc_handle;
} estc_ble_srv_char_handles_t;

/***
 * @brief Registers initialized characteristic
 *
 * @param [in] service - service which should include with characteristic
 *
 * @param [in] config - configuration of a characteristic which should be registered
 *
 * @param [out] handles - returns user description, client characteristic, server characteristic, value attributes handles
 *
 * @retval NRF_ERROR-INVALID_PARAM - Invalid permissions, service handle, UUID, or lengths
 *
 * @retval NRF_ERROR_INVALID_STATE - Invalid service context
 *
 * @retval NRF_ERROR_INVALID_FORBIDDEN - Forbidden UUID value supplied
 *
 * @retval NRF_ERROR_NO_MEM - no memory to register characteristic
 *
 * @retval NRF_ERROR_INVALID_DATA_SIZE - possible data size length exceeded BLE_GATTS_ATTRIBUTE_LENGTH
 *
 * @retval NRF_SUCCESS - characteristic registered successfully
 */
ret_code_t estc_ble_srv_char_register(estc_ble_service_t *service,
									  estc_ble_srv_char_cfg_t *config,
									  estc_ble_srv_char_handles_t *handles);

/**
 * @brief Send notification.
 * 
 * @param[in] conn_handle - connection handle which should recieve notification.
 * 
 * @param[in] value_handle - handle of the value which should is a subject of notification.
 * 
 * @param[in] data - pointer to a value which should be sent with notification.
 * 
 * @param[in] data_len - pointer to a variable representing length of the data.
*/
void estc_ble_srv_char_notify(uint16_t conn_handle, uint16_t value_handle, uint8_t *data, uint16_t *data_len);

/**
 * @brief Send indication.
 * 
 * @param[in] conn_handle - connection handle which should recieve indication.
 * 
 * @param[in] value_handle - handle of the value which should is a subject of indication.
 * 
 * @param[in] data - pointer to a value which should be sent with indication.
 * 
 * @param[in] data_len - pointer to a variable representing length of the data.
*/
void estc_ble_srv_char_indicate(uint16_t conn_handle, uint16_t value_handle, uint8_t *data, uint16_t *data_len);

#endif // ESTC_BLE_SERVICE_CHARACTERISTIC_H_