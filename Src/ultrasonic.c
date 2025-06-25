#include "delay.h"
#include "stm32f4xx.h"
#include "ultrasonic.h"
#include <stdint.h>


#define TRIG_PIN_OUTPUT		7	// PA7 (D11)
#define ECHO_PIN_INPUT		6	// PA6 (D12)

// Initialize ultrasonic sensor
void ultrasonic_init(void) {
	// Enable GPIOA clock
	RCC->AHB1ENR |= 1;	// Enable bit 0 (GPIOAEN)

	// Set PA6 as input (Echo)
	GPIOA->MODER &= ~(3 << ECHO_PIN_INPUT * 2);	// Clear register
	GPIOA->MODER |= (0 << ECHO_PIN_INPUT * 2);	// Set to input

	// Set PA7 as output (Trig)
	GPIOA->MODER &= ~(3 << TRIG_PIN_OUTPUT * 2);	// Clear register
	GPIOA->MODER |= (1 << TRIG_PIN_OUTPUT * 2);		// Set to output mode
}

// Sends 10us pulse to PA7 to trigger sensor
void send_trigger_pulse(void) {
	// Set PA7 to low for 2us to clear the line
	GPIOA->ODR &= ~(1 << TRIG_PIN_OUTPUT);
	delay_us(2);

	// Set PA7 to high for 10us, activates ultrasonic sensing
	GPIOA->ODR |= (1 << TRIG_PIN_OUTPUT);
	delay_us(10);

	// Set PA7 back to low
	GPIOA->ODR &= ~(1 << TRIG_PIN_OUTPUT);
}

int measure_echo_pulse(void) {
	// Timeouts for safety
	uint32_t timeout_rising_edge = 100000;
	uint32_t timeout_falling_edge = 100000;

	// Wait for rising edge
	while ( !(GPIOA->IDR & (1 << ECHO_PIN_INPUT) ) && --timeout_rising_edge ) {}
	if (timeout_rising_edge == 0) 	{ return 0; }

	TIM5->CNT = 0;										// Reset counter
	TIM5->CR1 |= TIM_CR1_CEN;							// Enable timer

	// Wait for falling edge
	while ( GPIOA->IDR &  (1 << ECHO_PIN_INPUT) && --timeout_falling_edge) 	{}
	TIM5->CR1 &= ~(TIM_CR1_CEN);						// Disable timer

	if (timeout_rising_edge == 0)	{ return 0; }

	return TIM5->CNT;
}

int ultrasonic_get_distance_cm(int duration_us) {
	if (duration_us == 0 || duration_us > 25000) return -1;  // Too far or timeout
	return duration_us / 58;
}

int ultrasonic_get_distance_in(int duration_us) {
	if (duration_us == 0 || duration_us > 25000) return -1;  // Too far or timeout
	return duration_us / 148;
}
