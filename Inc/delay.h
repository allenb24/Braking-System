#ifndef DELAY_H
#define DELAY_H

#include "stm32f4xx.h"
#include <stdint.h>


extern volatile uint8_t delay_done;

/**
 * Initializes TIM2 to generate a 1ms time base and sets up NVIC.
 */
void tim2_init(void);

/**
 * Starts a non-blocking delay using TIM2 interrupt.
 * Use `delay_done` flag to check when the delay finishes.
 *
 * @param ms Delay duration in milliseconds
 */
void delay_start(uint32_t ms);

// Blocking delay in ms
void delay_ms(uint32_t ms);

#endif
