/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 * @defgroup pwm_example_main main.c
 * @{
 * @ingroup pwm_example
 *
 * @brief PWM Example Application main file.
 *
 * This file contains the source code for a sample application using PWM.
 */

#include <stdio.h>
#include <string.h>

#include "nrf.h"
#include "nrf_drv_WS2812.h"
#include "nrf_delay.h"


int main(void)
{
    
    nrf_drv_WS2812_init(11);
    
    //create rainbow
    /*
    for(int i = 0; i < NR_OF_PIXELS/2; i++)
    {
        nrf_drv_WS2812_set_pixel(i, 255 - i*255/NR_OF_PIXELS/2, i*255/NR_OF_PIXELS/2, 0);
    }
    for(int i = NR_OF_PIXELS/2; i < NR_OF_PIXELS; i++)
    {
        nrf_drv_WS2812_set_pixel(i, 0, 255 - (i - NR_OF_PIXELS/2)*255/NR_OF_PIXELS/2, (i - NR_OF_PIXELS/2)*255/NR_OF_PIXELS/2);
    }
    */
    
    
    
    
    for (;;)
    {
        for(int i = 0; i < NR_OF_PIXELS; i++)
        {
            nrf_drv_WS2812_set_pixel(i, 255, 0, 0);
        }
        
        nrf_drv_WS2812_show();
        nrf_delay_ms(1000);
        
        for(int i = 0; i < NR_OF_PIXELS; i++)
        {
            nrf_drv_WS2812_set_pixel(i, 0, 255, 0);
        }
        
        nrf_drv_WS2812_show();
        nrf_delay_ms(1000);
        
        for(int i = 0; i < NR_OF_PIXELS; i++)
        {
            nrf_drv_WS2812_set_pixel(i, 0, 0, 255);
        }
        
        nrf_drv_WS2812_show();
        nrf_delay_ms(1000);
    }
}

/** @} */
