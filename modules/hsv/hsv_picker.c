#include "../gpio/led/led.h"
#include "hsv_picker.h"

#define PWM_CFG_RGB_TOP_VALUE       255
#define PWM_CFG_INDICATOR_TOP_VALUE 100
#define PWM_CFG_INDICATOR_STEP      1
#define INDICATOR_ARR_SIZE          PWM_CFG_INDICATOR_TOP_VALUE / PWM_CFG_INDICATOR_STEP + 1

static nrfx_pwm_t pwm_rgb = NRFX_PWM_INSTANCE(0);
static nrfx_pwm_t pwm_edit_indicator = NRFX_PWM_INSTANCE(1);

static nrf_pwm_values_individual_t rgb_values = 
{
	.channel_0 = 0, 
	.channel_1 = 0, 
	.channel_2 = 0, 
	.channel_3 = 0
};

static nrf_pwm_values_common_t edit_indicator_high_low[1] = {0};

static bool is_indicator_seq_inited = false;
static nrf_pwm_values_common_t volatile edit_indicator_values[INDICATOR_ARR_SIZE];

static nrf_pwm_sequence_t const seq_rgb =
{
	.values.p_individual = rgb_values,
	.length = NRF_PWM_VALUES_LENGTH(rgb_values),
	.repeats = 0,
	.end_delay = 0
};

// config of rgb leds
static nrfx_pwm_config_t const cfg_rgb = 
{
	.output_pins = 
	{
		LED_PCA10059_RED | NRFX_PWM_PIN_INVERTED,
		LED_PCA10059_GREEN | NRFX_PWM_PIN_INVERTED,
		LED_PCA10059_BLUE | NRFX_PWM_PIN_INVERTED,
		NRFX_PWM_PIN_NOT_USED
	},
	.irq_priority = APP_IRQ_PRIORITY_LOWEST,
	.base_clock = NRF_PWM_CLK_1MHz,
	.count_mode = NRF_PWM_MODE_UP,
	.top_value = PWM_CFG_RGB_TOP_VALUE,
	.load_mode = NRF_PWM_LOAD_INDIVIDUAL,
	.step_mode = NRF_PWM_STEP_AUTO
};

static nrf_pwm_sequence_t const edit_indicator_high_low_seq =
{
	.values = edit_indicator_high_low,
	.length = NRF_PWM_VALUES_LENGTH(edit_indicator_high_low),
	.repeats = 0,
	.end_delay = 0
};

static nrf_pwm_sequence_t const edit_indicator_seq =
{
	.values = edit_indicator_values,
	.length = NRF_PWM_VALUES_LENGTH(edit_indicator_values),
	.repeats = 0,
	.end_delay = 0
};

// config of indicator led when hue is edited & when hsv is not edited
static nrfx_pwm_config_t const cfg_edit_hue_ind =
{
	.output_pins = 
	{
		LED_PCA10059_YELLOW | NRFX_PWM_PIN_INVERTED,
		NRFX_PWM_PIN_NOT_USED,
		NRFX_PWM_PIN_NOT_USED,
		NRFX_PWM_PIN_NOT_USED
	},
	.irq_priority = APP_IRQ_PRIORITY_LOWEST,
	.base_clock = NRF_PWM_CLK_125kHz,
	.count_mode = NRF_PWM_MODE_UP,
	.top_value = PWM_CFG_INDICATOR_TOP_VALUE,
	.load_mode = NRF_PWM_LOAD_COMMON,
	.step_mode = NRF_PWM_STEP_AUTO,
};

// config of indicator led when saturation & brightness is edited
static nrfx_pwm_config_t const cfg_edit_sat_ind =
{
	.output_pins = 
	{
		LED_PCA10059_YELLOW,
		NRFX_PWM_PIN_NOT_USED,
		NRFX_PWM_PIN_NOT_USED,
		NRFX_PWM_PIN_NOT_USED
	},
	.irq_priority = APP_IRQ_PRIORITY_LOWEST,
	.base_clock = NRF_PWM_CLK_250kHz,
	.count_mode = NRF_PWM_MODE_UP_AND_DOWN,
	.top_value = PWM_CFG_INDICATOR_TOP_VALUE,
	.load_mode = NRF_PWM_LOAD_COMMON,
	.step_mode = NRF_PWM_STEP_AUTO,
};

static void hsv_picker_init_indicator_seq(void)
{
	uint16_t delay = 0;
	for (uint16_t i = 0; i < INDICATOR_ARR_SIZE; i++)
	{
		edit_indicator_values[i] = delay;
		delay += PWM_CFG_INDICATOR_STEP;
	}

	is_indicator_seq_inited = true;
}

void hsv_picker_init(uint16_t initial_hue, uint8_t initial_saturation, uint8_t initial_brightness)
{
	if (!is_indicator_seq_inited)
	{
		hsv_picker_init_indicator_seq();
	}

	nrfx_err_t err_code = nrfx_pwm_init(&pwm_rgb, &cfg_rgb, NULL);
	APP_ERROR_CHECK(err_code);

	err_code = nrfx_pwm_init(&pwm_edit_indicator, &cfg_edit_hue_ind, NULL);
	APP_ERROR_CHECK(err_code);
}