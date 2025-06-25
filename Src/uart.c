#include "stm32f4xx.h"
#include "uart.h"

void uart2_init(void) {
	// Enable GPIOA and USART2 clocks
	RCC->AHB1ENR |= (1 << 0);	// GPIOAEN
	RCC->APB1ENR |= (1 << 17);	// USART2EN

	// Set PA2 (D1) to Tx via AF
	GPIOA->MODER &= ~(3 << 2 * 2);	// Clear PA2 reg
	GPIOA->MODER |= (2 << 2 * 2);	// Set PA2 to Alternate Function

	// Set PA3 (D0) to Rx via AF
	GPIOA->MODER &= ~(3 << 3 * 2);	// Clear PA3 reg
	GPIOA->MODER |= (2 << 3 * 2);	// Set PA3 to Alternate Function

	// Set AF7 for USART2 on PA2/3
	GPIOA->AFR[0] &= ~(0xF << 2 * 4);
	GPIOA->AFR[0] |= (7 << 2 * 4);
	GPIOA->AFR[0] &= ~(0xF << 3 * 4);
	GPIOA->AFR[0] |= (7 << 3 * 4);

	// Config baud rate @ 84MHz
	USART2->BRR = 0x0683;		// 9600 baud rate (Modify accordingly)

	// Enable Tx/Rx
	USART2->CR1 |= (1 << 2);	// RE
	USART2->CR1 |= (1 << 3);	// TE

	USART2->CR1 |= (1 << 13);	// Enable USART
}

// Used by printf()
int __io_putchar(int ch) {
	USART2->DR = ch;
	while ( !(USART2->SR & (1 << 6)) ) {}	// Wait until transmission completes
	return ch;
}

// Receive character
int uart2_read(void) {
    while (!(USART2->SR & (1 << 5)));  // Wait until RXNE
    return USART2->DR;
}
