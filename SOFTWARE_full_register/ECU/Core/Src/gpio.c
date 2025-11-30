#include "gpio.h"
#include "stm32f4xx.h"  // For GPIO registers

// Initialize all GPIO pins
void MX_GPIO_Init(void)
{
  // Enable GPIO clocks
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOHEN;  // Enable clocks for ports A, B, H

  // Initial output levels: Reset BL and HORN pins
  GPIOA->BSRR = (BL_Pin << 16) | (HORN_Pin << 16);  // Reset pins (high bit for reset in BSRR)

  // Input pins: BSPD and TSMS on GPIOB
  GPIOB->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER13);  // Set to input mode (assuming pins 12 and 13 for BSPD/TSMS)
  GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR12 | GPIO_PUPDR_PUPDR13);  // No pull-up/down

  // Output pins: BL and HORN on GPIOA
  GPIOA->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER15);  // Clear mode (assuming pins 12 and 15)
  GPIOA->MODER |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER15_0);  // Set to output mode
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT12 | GPIO_OTYPER_OT15);  // Push-pull type
  GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR15);  // Low speed
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR12 | GPIO_PUPDR_PUPDR15);  // No pull
}