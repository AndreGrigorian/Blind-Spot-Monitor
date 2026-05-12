/**
 * @file PMOD_BTN_Interrupt.c
 *
 * @brief Source file for the PMOD_BTN Interrupt driver.
 *
 * This file contains the function definitions for the PMOD_BTN Interrupt driver.
 * It interfaces with the PMOD BTN module. The following pin is used:
 *  - PMOD BTN0 (PA2)
 *  - PMOD BTN1 (PA3)
 *  - PMOD BTN2 (PA4)
 *  - PMOD BTN3 (PA5)
 *
 * It configures the pins to trigger interrupts on rising edges. The PMOD BTN
 * push buttons operate in an active high configuration.
 *
 * @author: Andre Grigorian
 */

#include "PMOD_BTN_Interrupt.h"

// Declare pointer to the user-defined task
void (*PMOD_BTN_Task)(uint8_t pmod_btn_state);

void PMOD_BTN_Interrupt_Init(void(*task)(uint8_t))
{
	PMOD_BTN_Task = task;
	SYSCTL->RCGCGPIO |= 0x01;
	GPIOA->AFSEL &= ~0x3C;
	GPIOA->DIR &= ~0x3C;
	GPIOA->DEN |= 0x3C;
	GPIOA->PDR |= 0x3C;
	GPIOA->IS &= ~0x3C;
	GPIOA->IBE &= ~0x3C;
	GPIOA->IEV |= 0x3C;
	GPIOA->ICR |= 0x3C;
	GPIOA->IM |= 0x3C;
	NVIC->IPR[0] &= ~0x000000E0;
	NVIC->IPR[0] |= (3 << 5);
	NVIC->ISER[0] |= (1 << 0);
}

uint8_t PMOD_BTN_Read(void)
{
	return (GPIOA->DATA & 0x3C);
}

void GPIOA_Handler(void)
{
	if (GPIOA->MIS & 0x3C)
	{
		// Execute the user-defined function
		(*PMOD_BTN_Task)(PMOD_BTN_Read());
		
		// Acknowledge the interrupt from the PA2-5 pin
		GPIOA->ICR |= 0x3C;
	}
}

