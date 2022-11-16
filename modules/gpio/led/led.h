#ifndef APP_LED_H
#define APP_LED_H

#include <hal/nrf_gpio.h>

#define LED_SWITCH_ON_SIGNAL  0
#define LED_SWITCH_OFF_SIGNAL 1

#define LED_PCA10059_RED    NRF_GPIO_PIN_MAP(0, 8)
#define LED_PCA10059_GREEN  NRF_GPIO_PIN_MAP(1, 9)
#define LED_PCA10059_BLUE   NRF_GPIO_PIN_MAP(0, 12)
#define LED_PCA10059_YELLOW NRF_GPIO_PIN_MAP(0, 6)

void led_init_all(void);

#endif