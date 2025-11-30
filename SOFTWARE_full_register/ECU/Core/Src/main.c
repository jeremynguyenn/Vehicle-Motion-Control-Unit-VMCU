#include "main.h"
#include "can.h"
#include "tim.h"
#include "gpio.h"
#include "stm32f4xx.h"  // For core registers

#include "TeR_CAN.h"
#include "TeR_STATEMACHINE.h"

// Main entry point
int main(void)
{
  // Bare-metal initialization (replace HAL_Init)
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  // Set NVIC priority grouping (optional)
  SysTick_Config(SystemCoreClock / 1000);          // Configure SysTick for 1ms ticks (for delays)

  SystemClock_Config();  // Configure system clock

  MX_GPIO_Init();        // Initialize GPIOs
  MX_CAN1_Init();        // Initialize CAN1
  MX_CAN2_Init();        // Initialize CAN2
  MX_TIM2_Init();        // Initialize TIM2
  MX_TIM3_Init();        // Initialize TIM3
  MX_TIM5_Init();        // Initialize TIM5
  MX_TIM4_Init();        // Initialize TIM4

  initSCS(TIM5);         // Start system critical signals
  initCAN(CAN1, CAN2, TIM3, TIM2); // Start inverter and main CAN with timers
  initStateMachine(TIM4); // State machine connected to TIM4

  while (1) {            // Infinite loop
    // User code here
  }
}

// System clock configuration (bare-metal)
void SystemClock_Config(void)
{
  // Enable HSE oscillator
  RCC->CR |= RCC_CR_HSEON;  
  while ((RCC->CR & RCC_CR_HSERDY) == 0);  // Wait for HSE ready

  // Configure PLL
  RCC->PLLCFGR = (8 << RCC_PLLCFGR_PLLM_Pos) | (80 << RCC_PLLCFGR_PLLN_Pos) | (RCC_PLLCFGR_PLLP_0) | (2 << RCC_PLLCFGR_PLLQ_Pos);
  RCC->CR |= RCC_CR_PLLON;  // Enable PLL
  while ((RCC->CR & RCC_CR_PLLRDY) == 0);  // Wait for PLL ready

  // Set flash latency
  FLASH->ACR = FLASH_ACR_LATENCY_2WS;  

  // Configure clocks
  RCC->CFGR = RCC_CFGR_SW_PLL | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV2;  // Switch to PLL, set dividers
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait for switch
}

// Error handler
void Error_Handler(void)
{
  while(1);  // Infinite loop on error
}