/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Allen Benjamin
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

/*
 * DEFINE MACROS
 */
// Define base address for peripherals
#define PERIPH_BASE			(0X40000000UL)

// Define offset & base for AHB1/APB2 peripheral bus
#define ABH1_OFFSET			(0x00020000UL)
#define AHB1_BASE			(PERIPH_BASE + ABH1_OFFSET)
#define APB2_OFFSET			(0x00010000UL)

// Define offset & base for GPIOA
#define GPIOA_OFFSET		(0x0000UL)
#define GPIOA_BASE			(AHB1_BASE + GPIOA_OFFSET)	// 0x40020000

// Define offset & base for RCC
#define RCC_OFFSET			(0x3800UL)
#define RCC_AHB1_BASE		(AHB1_BASE + RCC_OFFSET)	// For GPIOA
#define RCC_APB2_BASE		(AHB1_BASE + RCC_OFFSET)	// This is because RCC is located in AHB1, thus we need that base

// Define offset & base for AHB1ENR (Enable clock register)
#define RCC_AHB1ENR_OFFSET	(0x30)
#define RCC_AHB1ENR			( *(volatile unsigned int*) (RCC_AHB1_BASE + RCC_AHB1ENR_OFFSET) )	// For GPIOA (0x40023830)
#define RCC_APB2ENR_OFFSET	(0x44)
#define RCC_APB2ENR			( *(volatile unsigned int*) (RCC_APB2_BASE + RCC_APB2ENR_OFFSET) )	// For TIM1 (0x40023844)

// Define offset for mode register (GPIOA)
#define MODER_OFFSET 		(0x00UL)
#define GPIOA_MODER			( *(volatile unsigned int*) (GPIOA_BASE + MODER_OFFSET) )

// Define offset for alternate low data register
#define AFRH_OFFSET			(0x24UL)

// Define base for GPIOA alternate function register
#define GPIOA_AFRH			( *(volatile unsigned int*) (GPIOA_BASE + AFRH_OFFSET) )

// Define TIM1 for PWM
#define TIM1_BASE			(0x40010000U)
#define TIM1_CR1			( *(volatile unsigned int*) (TIM1_BASE + 0x00) )	// Enables buffering and starts/stops timer. Key bits: CEN(0) and ARPE(7)
#define TIM1_CCMR1			( *(volatile unsigned int*) (TIM1_BASE + 0x18) )	/*
																				 * Capture/Compare mode register 1.
																				 * bits[6:4] = OC1M; 110 = PWM mode 1 (activating channel 1 for PWM; remains active as long as TIM1_CCR1 is active)
																				 * bits[3] = OC1PE (preload enable); 1 = Preload register on TIM1_CCR1 enabled.
																				*/

#define TIM1_CCER			( *(volatile unsigned int*) (TIM1_BASE + 0x20) )	// Capture/Compare enable register. Enable bit 2 to enable output to PA7 = TIM1_CH1N
#define TIM1_PSC			( *(volatile unsigned int*) (TIM1_BASE + 0x28) )	/*
																				 * Divides input clock before timer counts
																				 * APB2 Timer clock = 84 MHz (assuming HCLK = 84 MHz, aka the default)
																				 * 84MHz / 84 = 1MHz
																				 * So, 1 tick = 1us
																				*/

#define TIM1_ARR			( *(volatile unsigned int*) (TIM1_BASE + 0x2C) )	// Defines the period of the PWM (in ticks); 1 ticks = 1us = 20000 ticks = 20ms = 50Hz PWM
#define TIM1_CCR1			( *(volatile unsigned int*) (TIM1_BASE + 0x34) )	/* Capture/Compare register; sets duty cycle of the PWM
																				 * Will default to 1500 ticks (1.5ms), which is center position for servo
																				*/

#define TIM1_BDTR			( *(volatile unsigned int*) (TIM1_BASE + 0x44) )	// Break and dead-time register. bits[15] = MOE (Main Output Enable) = 1 to active output on TIM1


// Bit mask to enable GPIOA (bit 0)
#define GPIOAEN				(1U << 0)
#define TIM1EN				(1U << 0)

// Hardware sanity check with GPIO
// Define offset for output data register (ODR)
#define ODR_OFFSET   (0x14UL)
#define GPIOA_ODR    (*(volatile uint32_t*)(GPIOA_BASE + ODR_OFFSET))


/*
 * FUNCTIONS
 */

// Input duty cycle (1000-2000) = (0deg-180deg)
//void adjust_servo(uint16_t us) {
//	// TODO Add range checking (ensure between 1000-2000)
//	TIM1_CCR1 = us;
//}

int main(void)
{
	// Enable clocks
	RCC_AHB1ENR |= GPIOAEN;	// Enable GPIOA
	RCC_APB2ENR |= TIM1EN;	// Enable TIM1

	// Configure PA7 as alternate function (PWM = 10)
	GPIOA_MODER &= ~(3U << 16);	// Clear bits 16 and 17 (PA8)
	GPIOA_MODER |= (2U << 16);	// Set bits as 10

	// Enable PA8 as an alternate function low register (bits[31:28])
	// AF1: TIM1_CH1 = 0001
	GPIOA_AFRH &= ~(0xF << 0);	// Clear AFRH
	GPIOA_AFRH |= (0x1 << 0);	// Set AFRH as TIM1_CH1 (0001)

	/*
	 * TIM1 setup for PWM
	 * NOTE: SG90 Data Sheet: PW = 20ms (50Hz), duty cycle 1-2ms (1.5ms being neutral)
	 */
	// 84 MHz system clock
	TIM1_PSC = 84 - 1; 	// 84MHz / 84 = 1 MHz	(Subtract 1 because starting at 0)
	TIM1_ARR = 20000 - 1;	// PWM Period = 20ms (50Hz)
	TIM1_CCR1 = 1500;	// Set duty cycle

	// PWM Mode 1: Output Compare 1 Mode (OC1M), Output Compare 1 Preload Enable (OC1PE).
	// Set bits[6:4] = 110; bits[3] = 1;
	TIM1_CCMR1 |= (6 << 4);
	TIM1_CCMR1 |= (1 << 3);	// Set bits

	// Enable CH1 output (PA8)
	TIM1_CCER |= (1 << 0);	// Capture/compare 1 output enable

	TIM1_BDTR |= (1 << 15); // MOE = 1

	TIM1_CR1 |= (1 << 7);	/* ARPE: preload ARR (Auto reload register); buffers register
							 * This ensures the current cycle completes cleanly before moving on to the next value by...
							 * preventing the new value to take effect immediately, potentially causing timer resets mid-period.
							 * Basically, this keeps the period of the PWM stable
							*/

	// Start timer
	TIM1_CR1 |= (1 << 0);	// CEN = 1


	while (1) {
	    adjust_servo(1000); // 0 degrees
	    for (volatile int i = 0; i < 1000000; ++i);
	    adjust_servo(2000); // 180 degrees
	    for (volatile int i = 0; i < 1000000; ++i);

	        // 1) Enable GPIOA clock
	        RCC_AHB1ENR |= (1<<0);

	        // 2) Configure PA5 (LD2) as push-pull output
	        GPIOA_MODER = (GPIOA_MODER & ~(3U << (5*2))) | (1U << (5*2));
	            GPIOA_ODR |=  (1<<5);        // LD2 on
	            for (volatile int i = 0; i < 500000; i++);
	            GPIOA_ODR &= ~(1<<5);        // LD2 off
	            for (volatile int i = 0; i < 200000; i++);
	            GPIOA_ODR |=  (1<<5);        // LD2 on twice quickly
	            for (volatile int i = 0; i < 500000; i++);
	            GPIOA_ODR &= ~(1<<5);        // LD2 off
	            for (volatile int i = 0; i < 2000000; i++);
	}

}
