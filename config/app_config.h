#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#ifndef NRFX_SYSTICK_ENABLED
#define NRFX_SYSTICK_ENABLED 1
#endif // NRFX_SYSTICK_ENABLED

// <e> NRF_LOG_BACKEND_UART_ENABLED - nrf_log_backend_uart - Log UART backend
//==========================================================
#ifndef NRF_LOG_BACKEND_UART_ENABLED
#define NRF_LOG_BACKEND_UART_ENABLED 0
#endif
// <o> NRF_LOG_BACKEND_UART_TX_PIN - UART TX pin
#ifndef NRF_LOG_BACKEND_UART_TX_PIN
#define NRF_LOG_BACKEND_UART_TX_PIN 6
#endif

// <o> NRF_LOG_BACKEND_UART_BAUDRATE  - Default Baudrate

// <323584=> 1200 baud
// <643072=> 2400 baud
// <1290240=> 4800 baud
// <2576384=> 9600 baud
// <3862528=> 14400 baud
// <5152768=> 19200 baud
// <7716864=> 28800 baud
// <10289152=> 38400 baud
// <15400960=> 57600 baud
// <20615168=> 76800 baud
// <30801920=> 115200 baud
// <61865984=> 230400 baud
// <67108864=> 250000 baud
// <121634816=> 460800 baud
// <251658240=> 921600 baud
// <268435456=> 1000000 baud

#ifndef NRF_LOG_BACKEND_UART_BAUDRATE
#define NRF_LOG_BACKEND_UART_BAUDRATE 30801920
#endif

// <o> NRF_LOG_BACKEND_UART_TEMP_BUFFER_SIZE - Size of buffer for partially processed strings.
// <i> Size of the buffer is a trade-off between RAM usage and processing.
// <i> if buffer is smaller then strings will often be fragmented.
// <i> It is recommended to use size which will fit typical log and only the
// <i> longer one will be fragmented.

#ifndef NRF_LOG_BACKEND_UART_TEMP_BUFFER_SIZE
#define NRF_LOG_BACKEND_UART_TEMP_BUFFER_SIZE 64
#endif
// </e>

// <e> LOG_BACKEND_USB_ENABLED - log_backend_usb - Log USB backend
//==========================================================
#ifndef LOG_BACKEND_USB_ENABLED
#define LOG_BACKEND_USB_ENABLED 1
#endif

// <o> LOG_BACKEND_USB_TEMP_BUFFER_SIZE - Size of buffer for partially processed strings.
// <i> Size of the buffer is a trade-off between RAM usage and processing.
// <i> if buffer is smaller then strings will often be fragmented.
// <i> It is recommended to use size which will fit typical log and only the
// <i> longer one will be fragmented.
#ifndef LOG_BACKEND_USB_TMP_BUFFER_SIZE
#define LOG_BACKEND_USB_TMP_BUFFER_SIZE 64
#endif

// <o> LOG_BACKEND_USB_CDC_ACM_COMM_INTERFACE - CDC ACM COMM Interface number
#ifndef LOG_BACKEND_USB_CDC_ACM_COMM_INTERFACE
#define LOG_BACKEND_USB_CDC_ACM_COMM_INTERFACE 0
#endif

// <o> LOG_BACKEND_USB_CDC_ACM_DATA_INTERFACE - CDC ACM Data Interface number
#ifndef LOG_BACKEND_USB_CDC_ACM_DATA_INTERFACE
#define LOG_BACKEND_USB_CDC_ACM_DATA_INTERFACE 1
#endif

// <o> LOG_BACKEND_USB_CDC_ACM_COMM_EPIN - CDC ACM COMM IN endpoint number
#ifndef LOG_BACKEND_USB_CDC_ACM_COMM_EPIN
#define LOG_BACKEND_USB_CDC_ACM_COMM_EPIN 2
#endif

// <o> LOG_BACKEND_USB_CDC_ACM_DATA_EPIN - CDC ACM DATA IN endpoint number
#ifndef LOG_BACKEND_USB_CDC_ACM_DATA_EPIN
#define LOG_BACKEND_USB_CDC_ACM_DATA_EPIN 1
#endif

// <o> LOG_BACKEND_USB_CDC_ACM_DATA_EPOUT - CDC ACM DATA OUT endpoint number
#ifndef LOG_BACKEND_USB_CDC_ACM_DATA_EPOUT
#define LOG_BACKEND_USB_CDC_ACM_DATA_EPOUT 1
#endif

// <o> LOG_BACKEND_USB_INIT_STACK - Init the USB stack during USB log backend init procedure
#ifndef LOG_BACKEND_USB_INIT_STACK
#define LOG_BACKEND_USB_INIT_STACK 1
#endif

// <o> LOG_BACKEND_USB_UTILIZE_POWER_EVENTS - Start the USB backend based on power
#ifndef LOG_BACKEND_USB_UTILIZE_POWER_EVENTS
#define LOG_BACKEND_USB_UTILIZE_POWER_EVENTS 1
#endif

#ifndef NRF_DFU_APP_DATA_AREA_SIZE
#define NRF_DFU_APP_DATA_AREA_SIZE 20480
#endif // NRF_DFU_APP_DATA_AREA_SIZE

// <e> NRFX_NVMC_ENABLED - nrfx_nvmc - NVMC peripheral driver
//==========================================================
#ifndef NRFX_NVMC_ENABLED
#define NRFX_NVMC_ENABLED 1
#endif // NRFX_NVMC_ENABLED

#ifndef ESTC_MAX_LINE_SIZE
#define ESTC_MAX_LINE_SIZE 64
#endif // ESTC_MAX_LINE_SIZE

#endif // APP_CONFIG_H