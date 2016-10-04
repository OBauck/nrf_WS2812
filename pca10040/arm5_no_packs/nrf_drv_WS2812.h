

#ifndef NRF_DRV_WS2812_H__
#define NRF_DRV_WS2812_H__

#include <stdint.h>

#define NR_OF_PIXELS 100

void nrf_drv_WS2812_init(uint8_t pin);
void nrf_drv_WS2812_set_pixel(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue);
void nrf_drv_WS2812_show(void);

#endif //NRF_DRV_WS2812
