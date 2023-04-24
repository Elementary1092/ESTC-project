#ifndef ESTC_BLE_GAP_BOND_H_
#define ESTC_BLE_GAP_BOND_H_

#define ESTC_BLE_GAP_BOND_MEM_BUFF_SIZE 512
#define SEC_PARAM_BOND                  1                                       //!< Perform bonding.
#define SEC_PARAM_MITM                  0                                       //!< Man In The Middle protection not required.
#define SEC_PARAM_LESC                  0                                       //!< LE Secure Connections not enabled.
#define SEC_PARAM_KEYPRESS              0                                       //!< Keypress notifications not enabled.
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    //!< No I/O capabilities.
#define SEC_PARAM_OOB                   0                                       //!< Out Of Band data not available.
#define SEC_PARAM_MIN_KEY_SIZE          7                                       //!< Minimum encryption key size.
#define SEC_PARAM_MAX_KEY_SIZE          16                                      //!< Maximum encryption key size.

void estc_ble_gap_bond_init(void);

#endif // ESTC_BLE_GAP_BOND_H_