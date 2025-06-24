#include "delay.h"
#include <stdint.h>


volatile uint8_t delay_done = 0;

// Initialize TIM2 (on APB1 bus)
void tim2_init(void) {
	RCC->APB1ENR |= 1 << 0;	// Enable peripheral clock TIM2 from APB1

    // Set the prescaler (PSC) to divide the 84MHz system clock
    // 84,000,000 / 8400 = 10,000 counts per second -> 1 tick = 0.1 ms
    TIM2->PSC = 8400 - 1;  // PSC is zero-based, so subtract 1
	TIM2->ARR = 10 - 1;		// 10 ticks = 1 ms (also zero-based)
	TIM2->EGR |= 1 << 0;	// Force update generation to immediately load PSC and ARR

	NVIC_EnableIRQ(TIM2_IRQn);	// Enable TIM2 interrupt in NVIC

	TIM2->CR1 = 0 << 0;		// Keep disabled unless activated by another function
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

// Initialize TIM5 (APB1 bus)
void tim5_init(void) {
	RCC->APB1ENR |= 1 << 3;	// Enable peripheral clock for TIM5

    // Set the prescaler (PSC) to divide the 84MHz system clock
    // 84,000,000 / 84 = 1,000,000 counts per second -> 1 tick = 1 us
	TIM5->PSC = 84 - 1;	// PSC is zero-based, so subtract 1
	TIM5->ARR = 0xFFFFFFFF;		// 1 tick = 1us
	TIM5->EGR |= 1 << 0;	// Force update generation to immediately load PSC and ARR

// Don't need to enable NVIC because we aren't using interrupts, just polling TIM5->CNT manually
//	NVIC_EnableIRQ(TIM5_IRQn);	// Enable TIM5 interrupt in NVIC

	TIM5->CR1 = 0 << 0;	// Keep disabled unless activated by another function
}

// No need for interrupts as we are simply monitoring cycle count
void TIM5_IRQHandler(void) {}

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

void delay_ms(uint32_t ms) {
	delay_start(ms);
	while (!delay_done);  // Blocking wait
}

void delay_us(uint32_t us) {
	for (volatile uint32_t i = 0; i < us * 8; i++) {}	// Multiply by 8 to match F411RE's 84MHz clock
}
