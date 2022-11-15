#ifndef HSV_PICKER_H
#define HSV_PICKER_H

#include <nrfx_pwm.h>
#include <nrf_drv_pwm.h>
#include <app_util_platform.h>

void hsv_picker_init(uint16_t initial_hue, uint8_t initial_saturation, uint8_t initial_brightness);

#endif
