#include "delay.h"
#include <stdint.h>


volatile uint8_t delay_done = 0;

// Initialize TIM2 (on APB1 bus)
void tim2_init(void) {
	// Enable peripheral clock TIM2 from APB1
	RCC->APB1ENR |= 1 << 0;

    // Set the prescaler (PSC) to divide the 84MHz system clock
    // 84,000,000 / 8400 = 10,000 counts per second -> 1 tick = 0.1 ms
    TIM2->PSC = 8400 - 1;  // PSC is zero-based, so subtract 1
	TIM2->ARR = 10 - 1;		// 10 ticks = 1 ms (also zero-based)
	TIM2->EGR |= 1 << 0;	// Force update generation to immediately load PSC and ARR

	NVIC_EnableIRQ(TIM2_IRQn);	// Enable TIM2 interrupt in NVIC

	TIM2->CR1 = 0 << 0;		// Keep timer disabled for now (will enable in delay_ms())
}

// Start a non-blocking delay using TIM2 interrupt
void delay_start(uint32_t ms) {
	delay_done = 0;	// Clear flag
	TIM2->CNT = 0;	// Reset counter
	TIM2->ARR = ms * 10 - 1;	// Reconfigure ARR for ms delay (10 ticks per ms)

	// Clear update interrupt flag (UIF)
	// This ensure that we wait for a new update
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->DIER |= TIM_DIER_UIE;	// Enable update interrupt
	TIM2->CR1 |= TIM_CR1_CEN;	// Start timer
}

void TIM2_IRQHandler(void) {
	// If we've received an interrupt from TIM2
	if (TIM2->SR & TIM_SR_UIF) {
		TIM2->SR &= ~TIM_SR_UIF;	// Clear flag
		TIM2->CR1 &= ~TIM_CR1_CEN;	// Stop timer
		TIM2->DIER &= ~TIM_DIER_UIE;	// Disable interrupt
		delay_done = 1;	// Set completion flag
	}
}

void delay_ms(uint32_t ms) {
	delay_start(ms);
	while (!delay_done);  // Blocking wait
}
