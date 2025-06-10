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
#define APB2_BASE			(PERIPH_BASE + APB2_OFFSET)

// Define offset & base for GPIOA
#define GPIOA_OFFSET		(0x0000UL)
#define GPIOA_BASE			(ABH1_BASE + GPIOA_OFFSET)	// 0x40020000

// Define offset & base for RCC
#define RCC_OFFSET			(0x3800UL)
#define RCC_AHB1_BASE		(AHB1_BASE + RCC_OFFSET)
#define RCC_APB2_BASE		(AHB1_BASE + RCC_OFFSET)	// This is because RCC is located in AHB1, thus we need that base

// Define offset & base for AHB1ENR (Enable clock register)
#define RCC_AHB1ENR_OFFSET	(0x30)
#define RCC_AHB1ENR			( *(volatile unsigned int*) (RCC_AHB1_BASE + RCC_AHB1ENR_OFFSET) )
#define RCC_APB2ENR_OFFSET	(0x44)
#define RCC_APB2ENR			( *(volatile unsigned int*) (RCC_APB2_BASE + RCC_APB2ENR_OFFSET) )

// Define offset for mode register (GPIOA)
#define MODER_OFFSET 		(0x00UL)
#define GPIOA_MODER			( *(volatile unsigned int*) (GPIOA_BASE + MODER_OFFSET) )

// Define offset for alternate low data register
#define AFRL_OFFSET			(0x20UL)

// Define base for GPIOA alternate function register
#define GPIOA_AFRL			( *(volatile unsigned int*) (GPIOA_BASE + AFRL_OFFSET) )

// Define TIM1
#define TIM1_BASE			(0x40010000U)

// Bit mask to enable GPIOA (bit 0)
#define GPIOAEN				(1U << 0)
#define TIM1EN				(1U << 0)

// Bit mask for Pin 7
#define PIN7				(1U << 7)

int main(void)
{
	// Enable clocks
	RCC_AHB1ENR |= GPIOAEN;	// GPIOA Enable
	RCC_APB2ENR |= TIM1EN;	// TIM1 Enable

	// Configure PA7 as alternate function (PWM = 10)
	GPIOA_MODER &= ~(3U << 14);	// Clear bits 14 and 15
	GPIOA_MODER |= (2U << 14);	// Set bits as 10

	// Enable PA7 as an alternate function low register (bits 31:28)
	// AF1: TIM1/2 = 0001
	GPIOA_AFRL &= ~(0xF << 28);	// Clear AFRL7
	GPIOA_AFRL |= (0x1 << 28);	// Set AFRL7 as TIM1 (0001)


}
