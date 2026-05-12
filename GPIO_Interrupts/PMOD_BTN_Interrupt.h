/**
 * @file PMOD_BTN_Interrupt.h
 *
 * @brief Header file for the PMOD BTN Interrupt driver.
 *
 * This file contains the function definitions for the PMOD BTN Interrupt driver.
 * It interfaces with the PMOD BTN module. The following pins are used:
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

#include "TM4C123GH6PM.h"

// Declare pointer to the user-defined task
extern void (*PMOD_BTN_Task)(uint8_t pmod_btn_state);

/**
 * @brief Initializes interrupts for the PMOD BTN push buttons using Port A.
 *
 * This function initializes edge-triggered interrupts for the PMOD BTN module
 * connected to the following pins:
 *  - PMOD BTN0 (PA2)
 *  - PMOD BTN1 (PA3)
 *  - PMOD BTN2 (PA4)
 *  - PMOD BTN3 (PA5)
 *
 * It configures the specified pins (PA2 to PA5) to trigger interrupts on rising edges.
 * When an interrupt occurs, the provided task function is executed with the current
 * PMOD BTN status. Interrupt priority is set to 3 for GPIO Port A.
 *
 * @param task A pointer to the user-defined function to be executed upon button interrupts.
 *
 * @return None
 */
void PMOD_BTN_Interrupt_Init(void(*task)(uint8_t));

/**
 * @brief Reads the current status of the PMOD BTN module.
 *
 * This function reads the logic states of PA2 to PA5 and returns them as a 4-bit value.
 *
 * @param None
 *
 * @return A 4-bit value representing the state of PMOD BTN0 to PMOD BTN3.
 */
uint8_t PMOD_BTN_Read(void);

/**
 * @brief The interrupt service routine (ISR) for GPIO Port A.
 *
 * This function is the interrupt service routine (ISR) for GPIO Port A.
 * It checks if an interrupt has been triggered by PA2 to PA5, and if so,
 * it executes the user-defined task function with the current PMOD BTN status.
 * After executing the task function, it acknowledges and clears the interrupt.
 *
 * @param None
 *
 * @return None
 */
void GPIOA_Handler(void);