//#include "main.h"
//#include "pir.h"
//
//#define PIR_PIN			0	// PA0
//#define PIR_PIN_MASK	(1U << PIR_PIN)
//
//void pir_init(void) {
//	// Enable GPIO Peripheral clock
//	RCC->AHB1ENR |= (1U << 0); 	// Bit 0 = GPIOAEN
//
//	// Set PA0 as input by configuring GPIOA_MODER
//	GPIOA->MODER &= ~(3U << PIR_PIN * 2);	// Set bits to 00 (input)
//}
//
//uint8_t pir_read(void) {
//	// If signal is HIGH return 1; else return 0
//	return (GPIOA->IDR & PIR_PIN_MASK) ? 1 : 0;	// IDR has 16 bits with input values from the corresponding input port
//}
