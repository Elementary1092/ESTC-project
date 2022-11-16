#include <app_timer.h>
#include <app_error.h>

#include "rtc.h"

void timer_rtc_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}