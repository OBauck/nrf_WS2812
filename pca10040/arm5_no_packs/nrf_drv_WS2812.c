

#include <stdio.h>
#include <string.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_drv_WS2812.h"

#define RESET_ZEROS_AT_START    41

//#define PERIOD_TICKS            20      //20/16MHz = 1.25us (should be 1.25us +-150ns)
//#define ONE_HIGH_TICKS          14      //14/16MHz = 0.875us (should be 0.9us +-150ns)
//#define ZERO_HIGH_TICKS         6       //6/16MHz = 0.375us (should be 0.35us +-150ns)

//faster
#define PERIOD_TICKS            18      //20/16MHz = 1.125us (should be 1.25us +-150ns)
#define ONE_HIGH_TICKS          13      //14/16MHz = 0.8125us (should be 0.9us +-150ns)
#define ZERO_HIGH_TICKS         5       //6/16MHz = 0.3125us (should be 0.35us +-150ns)

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} nrf_drv_WS2812_pixel_t;

//total ram usage (in bytes) is approximately 3*NR_OF_PIXELS + 24*NUMBER_OF_PIXELS*2 + (RESET_ZEROS_AT_START+1)*2 = NR_OF_PIXELS * 51 + 84

static nrf_drv_WS2812_pixel_t pixels[NR_OF_PIXELS];
static uint16_t WS2812_pwm_seq[NR_OF_PIXELS * 24 + RESET_ZEROS_AT_START + 1];     //RESET signal + 24 bits per pixel + one pwm cycle to set the output low at the end

void nrf_drv_WS2812_init(uint8_t pin)
{
    nrf_gpio_cfg_output(pin);
    nrf_gpio_pin_clear(pin);
    
    NRF_PWM0->PSEL.OUT[0] = (pin << PWM_PSEL_OUT_PIN_Pos) | 
                            (PWM_PSEL_OUT_CONNECT_Connected <<
                                                     PWM_PSEL_OUT_CONNECT_Pos);
    
    NRF_PWM0->ENABLE      = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
    NRF_PWM0->MODE        = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    NRF_PWM0->PRESCALER   = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
    
    NRF_PWM0->COUNTERTOP  = (PERIOD_TICKS << PWM_COUNTERTOP_COUNTERTOP_Pos); //1.25 usec
    NRF_PWM0->LOOP        = (1 << PWM_LOOP_CNT_Pos);
    NRF_PWM0->DECODER   = (PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) | 
                          (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
    
    NRF_PWM0->SEQ[1].PTR  = ((uint32_t)(WS2812_pwm_seq) << PWM_SEQ_PTR_PTR_Pos);
    NRF_PWM0->SEQ[1].CNT  = ((sizeof(WS2812_pwm_seq) / sizeof(uint16_t)) << PWM_SEQ_CNT_CNT_Pos);
    NRF_PWM0->SEQ[1].REFRESH  = 0; 

    for(int i = 0; i < sizeof(WS2812_pwm_seq)/sizeof(uint16_t); i++)
    {
        WS2812_pwm_seq[i] = 0x8000;
    }
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
                WS2812_pwm_seq[RESET_ZEROS_AT_START + i*24 + j] = ONE_HIGH_TICKS | 0x8000;
            }
            else
            {
                WS2812_pwm_seq[RESET_ZEROS_AT_START + i*24 + j] = ZERO_HIGH_TICKS | 0x8000;
            }
        }
        for(uint8_t j = 0; j < 8; j++)
        {
            if( (pixels[i].red << j) & 0x80)
            {
                WS2812_pwm_seq[RESET_ZEROS_AT_START + i*24 + j + 8] = ONE_HIGH_TICKS | 0x8000;
            }
            else
            {
                WS2812_pwm_seq[RESET_ZEROS_AT_START + i*24 + j + 8] = ZERO_HIGH_TICKS | 0x8000;
            }
        }
        for(uint8_t j = 0; j < 8; j++)
        {
            if( (pixels[i].blue << j) & 0x80)
            {
                WS2812_pwm_seq[RESET_ZEROS_AT_START + i*24 + j + 16] = ONE_HIGH_TICKS | 0x8000;
            }
            else
            {
                WS2812_pwm_seq[RESET_ZEROS_AT_START + i*24 + j + 16] = ZERO_HIGH_TICKS | 0x8000;
            }
        }
    }
    NRF_PWM0->TASKS_SEQSTART[1] = 1;
}
