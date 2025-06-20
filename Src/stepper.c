#include "stepper.h"
#include "stm32f4xx.h"
#include "delay.h"
#include <stdint.h>

// Define pin mappings
#define IN1_PIN		10 	// PA10 (D2)
#define IN2_PIN		3	// PB3 	(D3)
#define IN3_PIN		5	// PB5	(D4)
#define IN4_PIN		4	// PB4	(D5)

// Stepper sequence table (8-step half-stepping)
static const uint8_t step_sequence[8][4] = {
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 1, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 1, 1},
		{0, 0, 0, 1},
		{1, 0, 0, 1}
};

void stepper_init(void) {
	// Enable GPIOA/B clocks (AHB1)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

	// Set PA10 (D2) as output to IN1
	GPIOA->MODER &= ~(3 << (IN1_PIN) * 2);	// Clear bits
	GPIOA->MODER |= (1 << (IN1_PIN) * 2);	// Set PA10 as output

	// Set PB3/5/4 as output to pins 3/5/4, respectively
	GPIOB->MODER &= ~(3 << (IN2_PIN) * 2) | ~(3 << (IN3_PIN) * 2) | ~(3 << (IN4_PIN) * 2); 	// Clear bits
	GPIOB->MODER |= (1 << (IN2_PIN) * 2) | (1 << (IN3_PIN) * 2) | (1 << (IN4_PIN) * 2);		// Set PB3/5/4 as output
}

void stepper_step(int steps, int dir, int delay_between_steps) {
	for (int i = 0; i < steps; i++) {
		int idx = (dir > 0) ? (i % 8) : (7 - (i % 8));	// Get the number of step based on direction
		uint8_t *seq = (uint8_t*)step_sequence[idx];	// Pointing to rows (0-7)

		// IN1 (PA10)
		if (seq[0]) {	GPIOA->ODR |= (1 << IN1_PIN);	}
		else {	GPIOA->ODR &= ~(1 << IN1_PIN);	}

		// IN2 (PB3)
		if (seq[1]) {	GPIOB->ODR |= (1 << IN2_PIN);	}
		else {	GPIOB->ODR &= ~(1 << IN2_PIN);	}

		// IN3 (PB5)
		if (seq[2]) {	GPIOB->ODR |= (1 << IN3_PIN);	}
		else {	GPIOB->ODR &= ~(1 << IN3_PIN);	}

		// IN4 (PB4)
		if (seq[3]) {	GPIOB->ODR |= (1 << IN4_PIN);	}
		else {	GPIOB->ODR &= ~(1 << IN4_PIN);	}

		delay_ms(delay_between_steps);
	}
}
