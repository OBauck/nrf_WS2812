

#include <stdio.h>
#include <string.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"
#include "nrf_drv_WS2812.h"
#include "nrf_drv_pwm.h"

//slow
//#define RESET_ZEROS_AT_START    41
//#define PERIOD_TICKS            20      //20/16MHz = 1.25us (should be 1.25us +-150ns)
//#define ONE_HIGH_TICKS          14      //14/16MHz = 0.875us (should be 0.9us +-150ns)
//#define ZERO_HIGH_TICKS         6       //6/16MHz = 0.375us (should be 0.35us +-150ns)

//fast
#define RESET_ZEROS_AT_START    45
#define PERIOD_TICKS            18      //20/16MHz = 1.125us (should be 1.25us +-150ns)
#define ONE_HIGH_TICKS          13      //14/16MHz = 0.8125us (should be 0.9us +-150ns)
#define ZERO_HIGH_TICKS         5       //6/16MHz = 0.3125us (should be 0.35us +-150ns)

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} nrf_drv_WS2812_pixel_t;

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);

static nrf_pwm_values_common_t m_seq_values[NR_OF_PIXELS * 24 + RESET_ZEROS_AT_START + 1];     //RESET signal + 24 bits per pixel + one pwm cycle to set the output low at the end
static nrf_pwm_sequence_t const m_seq =
{
    .values.p_common     = m_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

//total ram usage (in bytes) is approximately 3*NR_OF_PIXELS + 24*NUMBER_OF_PIXELS*2 + (RESET_ZEROS_AT_START+1)*2 = NR_OF_PIXELS * 51 + 84

static nrf_drv_WS2812_pixel_t pixels[NR_OF_PIXELS];

void nrf_drv_WS2812_init(uint8_t pin)
{
    nrf_gpio_cfg_output(pin);
    nrf_gpio_pin_clear(pin);
    
    uint32_t err_code;
    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            pin, // channel 0
            NRF_DRV_PWM_PIN_NOT_USED, // channel 1
            NRF_DRV_PWM_PIN_NOT_USED, // channel 2
            NRF_DRV_PWM_PIN_NOT_USED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOW,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = PERIOD_TICKS,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    
    err_code = nrf_drv_pwm_init(&m_pwm0, &config0, NULL);
    APP_ERROR_CHECK(err_code);
    
    for(int i = 0; i < NRF_PWM_VALUES_LENGTH(m_seq_values); i++)
    {
        m_seq_values[i] = ONE_HIGH_TICKS;
    }
		
	for(int i = 0; i < RESET_ZEROS_AT_START; i++)
	{
		m_seq_values[i] = 0x8000;
	}
	
	m_seq_values[NR_OF_PIXELS * 24 + RESET_ZEROS_AT_START] = 0x8000;
	
	nrf_drv_pwm_simple_playback(&m_pwm0, &m_seq, 1, 0);
}

void nrf_drv_WS2812_set_pixel(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue)
{
    pixels[pixel].red = red;
    pixels[pixel].green = green;
    pixels[pixel].blue = blue;
}

void nrf_drv_WS2812_show(void)
{
    //translate pixels array to pwm sequence array
    
    for(uint8_t i = 0; i < (sizeof(pixels)/sizeof(nrf_drv_WS2812_pixel_t)) ; i++)
    {
        for(uint8_t j = 0; j < 8; j++)
        {
            if( (pixels[i].green << j) & 0x80)
            {
                m_seq_values[RESET_ZEROS_AT_START + i*24 + j] = ONE_HIGH_TICKS | 0x8000;
            }
            else
            {
                m_seq_values[RESET_ZEROS_AT_START + i*24 + j] = ZERO_HIGH_TICKS | 0x8000;
            }
        }
        for(uint8_t j = 0; j < 8; j++)
        {
            if( (pixels[i].red << j) & 0x80)
            {
                m_seq_values[RESET_ZEROS_AT_START + i*24 + j + 8] = ONE_HIGH_TICKS | 0x8000;
            }
            else
            {
                m_seq_values[RESET_ZEROS_AT_START + i*24 + j + 8] = ZERO_HIGH_TICKS | 0x8000;
            }
        }
        for(uint8_t j = 0; j < 8; j++)
        {
            if( (pixels[i].blue << j) & 0x80)
            {
                m_seq_values[RESET_ZEROS_AT_START + i*24 + j + 16] = ONE_HIGH_TICKS | 0x8000;
            }
            else
            {
                m_seq_values[RESET_ZEROS_AT_START + i*24 + j + 16] = ZERO_HIGH_TICKS | 0x8000;
            }
        }
    }
    
    nrf_drv_pwm_simple_playback(&m_pwm0, &m_seq, 1, 0);
}
