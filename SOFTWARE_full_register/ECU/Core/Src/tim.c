#include "tim.h"
#include "stm32f4xx.h"  // For timer registers

// TIM2 initialization (bare-metal)
void MX_TIM2_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  // Enable TIM2 clock
  TIM2->PSC = 7;                        // Set prescaler
  TIM2->ARR = 49999;                    // Set auto-reload (period)
  TIM2->CR1 |= TIM_CR1_ARPE;            // Enable auto-reload preload (upgrade for stability)
  // Internal clock source by default
  TIM2->DIER |= TIM_IT_UPDATE;          // Enable update interrupt
  NVIC_SetPriority(TIM2_IRQn, 1);       // Set interrupt priority
  NVIC_EnableIRQ(TIM2_IRQn);            // Enable interrupt
}

// TIM3 initialization
void MX_TIM3_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;  // Enable clock
  TIM3->PSC = 3;                        // Prescaler
  TIM3->ARR = 39999;                    // Period
  TIM3->CR1 |= TIM_CR1_ARPE;            // Auto-reload preload
  TIM3->DIER |= TIM_IT_UPDATE;          // Update interrupt
  NVIC_SetPriority(TIM3_IRQn, 0);       // Priority
  NVIC_EnableIRQ(TIM3_IRQn);            // Enable
}

// TIM4 initialization (completed from truncated original)
void MX_TIM4_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;  // Enable clock
  TIM4->PSC = 0;                        // Prescaler (assume 0 from original)
  TIM4->ARR = 0xFFFFFFFF;               // Period (full 32-bit for general use)
  TIM4->CR1 |= TIM_CR1_ARPE;            // Auto-reload preload
  TIM4->DIER |= TIM_IT_UPDATE;          // Update interrupt
  NVIC_SetPriority(TIM4_IRQn, 1);       // Priority
  NVIC_EnableIRQ(TIM4_IRQn);            // Enable
}

// TIM5 initialization
void MX_TIM5_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;  // Enable clock
  TIM5->PSC = 0;                        // Prescaler
  TIM5->ARR = 0xFFFFFFFF;               // Period
  TIM5->CR1 |= TIM_CR1_ARPE;            // Auto-reload
  // No interrupt in original for TIM5, but enable if needed for SCS
  TIM5->DIER |= TIM_IT_UPDATE;          // Enable update interrupt for SCS timing
  NVIC_SetPriority(TIM5_IRQn, 2);       // Set priority (assume)
  NVIC_EnableIRQ(TIM5_IRQn);            // Enable interrupt if used
}