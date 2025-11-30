#include "can.h"
#include "stm32f4xx.h"  // For register definitions

// CAN1 initialization function (bare-metal)
void MX_CAN1_Init(void)
{
  // Enable CAN1 clock in RCC
  RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

  // GPIO configuration for CAN1 (PB8 RX, PB9 TX)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // Enable GPIOB clock
  GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);  // Clear mode bits
  GPIOB->MODER |= (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1);  // Set to alternate function mode
  GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9);  // Clear AF bits
  GPIOB->AFR[1] |= (9 << GPIO_AFRH_AFSEL8_Pos) | (9 << GPIO_AFRH_AFSEL9_Pos);  // Set AF9 for CAN1

  // CAN1 configuration
  CAN1->MCR = CAN_MCR_INRQ;  // Enter initialization mode
  while ((CAN1->MSR & CAN_MSR_INAK) == 0);  // Wait for initialization mode acknowledgment

  CAN1->BTR = (5 - 1) << CAN_BTR_BRP_Pos | (13 - 1) << CAN_BTR_TS1_Pos | (2 - 1) << CAN_BTR_TS2_Pos | (1 - 1) << CAN_BTR_SJW_Pos;  // Set timing: Prescaler 5, BS1 13, BS2 2, SJW 1
  CAN1->MCR &= ~CAN_MCR_INRQ;  // Exit initialization mode
  while ((CAN1->MSR & CAN_MSR_INAK) != 0);  // Wait for normal mode acknowledgment

  // Enable FIFO0 message pending interrupt
  CAN1->IER |= CAN_IER_FMPIE0;  
  NVIC_SetPriority(CAN1_RX0_IRQn, 0);  // Set interrupt priority
  NVIC_EnableIRQ(CAN1_RX0_IRQn);       // Enable interrupt
}

// CAN2 initialization function (bare-metal)
void MX_CAN2_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_CAN2EN | RCC_APB1ENR_CAN1EN;  // Enable CAN2 and CAN1 clocks (CAN2 shares with CAN1)

  // GPIO for CAN2 (PB5 RX, PB6 TX)
  GPIOB->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6);  // Clear mode bits
  GPIOB->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1);  // Set to AF mode
  GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL6);  // Clear AF bits
  GPIOB->AFR[0] |= (9 << GPIO_AFRL_AFSEL5_Pos) | (9 << GPIO_AFRL_AFSEL6_Pos);  // Set AF9

  // CAN2 configuration
  CAN2->MCR = CAN_MCR_INRQ;  // Enter init mode
  while ((CAN2->MSR & CAN_MSR_INAK) == 0);  // Wait
  CAN2->BTR = (5 - 1) << CAN_BTR_BRP_Pos | (6 - 1) << CAN_BTR_TS1_Pos | (1 - 1) << CAN_BTR_TS2_Pos | (1 - 1) << CAN_BTR_SJW_Pos;  // Timing parameters
  CAN2->MCR &= ~CAN_MCR_INRQ;  // Exit init mode
  while ((CAN2->MSR & CAN_MSR_INAK) != 0);  // Wait

  CAN2->IER |= CAN_IER_FMPIE0;  // Enable interrupt
  NVIC_SetPriority(CAN2_RX0_IRQn, 1);  // Set priority
  NVIC_EnableIRQ(CAN2_RX0_IRQn);       // Enable interrupt
}