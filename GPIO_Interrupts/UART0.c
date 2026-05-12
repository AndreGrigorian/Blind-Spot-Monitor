/**
 * @file UART0.c
 *
 * @brief Source code for the UART0 driver.
 *
 * This file contains the function definitions for the UART0 driver.
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 *   - Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @note Assumes that the frequency of the system clock is 50 MHz.
 *
 * @author
 */

#include "UART0.h"
#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include <string.h>
#include <stdint.h>
#include "GPIO.h"


void UART0_Init(void)
{
	
	SYSCTL->RCGCUART |= 0X01;
	SYSCTL-> RCGCGPIO |= 0X01;
	UART0->CTL &= ~0X0001;
	UART0->CTL &= ~0X0020;
	UART0->IBRD = 27;
	UART0->FBRD = 8;
	
	// Set data length to 8 bits (WLEN = 0x3)
	UART0->LCRH |= 0x60;

	// Enable FIFO (FEN bit)
	UART0->LCRH |= 0x10;

	// One stop bit (clear STP2)
	UART0->LCRH &= ~0x08;

	// Disable parity (clear PEN)
	UART0->LCRH &= ~0x02;

	// Enable UART (UARTEN bit)
	UART0->CTL |= 0x01;

	// Enable alternate function for PA0 and PA1
	GPIOA->AFSEL |= 0x03;

	// Clear PCTL for PA0, PA1
	GPIOA->PCTL &= ~0x000000FF;

	// Set PA1 = U0TX (0x1), PA0 = U0RX (0x1)
	GPIOA->PCTL |= 0x00000010;
	GPIOA->PCTL |= 0x00000001;

	// Enable digital for PA0 and PA1
	GPIOA->DEN |= 0x03;
}

char UART0_Input_Character(void)
{
    while((UART0->FR & UART0_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);

    return (char)(UART0->DR & 0xFF);
}

void UART0_Output_Character(char data)
{
    while((UART0->FR & UART0_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);

    UART0->DR = data;
}

void UART0_Input_String(char *buffer_pointer, uint16_t buffer_size)
{
    int length = 0;
    char character = UART0_Input_Character();

    while(character != UART0_CR)
    {
        if (character == UART0_BS)
        {
            if (length)
            {
                buffer_pointer--;
                length--;
                UART0_Output_Character(UART0_BS);
            }
        }
        else if(length < buffer_size)
        {
            *buffer_pointer = character;
            buffer_pointer++;
            length++;
            UART0_Output_Character(character);
        }

        character = UART0_Input_Character();
    }

    *buffer_pointer = 0;
}

void UART0_Output_String(char *pt)
{
    while(*pt)
    {
        UART0_Output_Character(*pt);
        pt++;
    }
}

uint32_t UART0_Input_Unsigned_Decimal(void)
{
    uint32_t number = 0;
    uint32_t length = 0;
    char character = UART0_Input_Character();

    while(character != UART0_CR)
    {
        if ((character >= '0') && (character <= '9'))
        {
            number = (10 * number) + (character - '0');
            length++;
            UART0_Output_Character(character);
        }
        else if ((character == UART0_BS) && length)
        {
            number /= 10;
            length--;
            UART0_Output_Character(character);
        }

        character = UART0_Input_Character();
    }

    return number;
}


void UART0_Output_Unsigned_Decimal(uint32_t n)
{
    if (n >= 10)
    {
        UART0_Output_Unsigned_Decimal(n / 10);
        n = n % 10;
    }

    UART0_Output_Character(n + '0');
}

uint32_t UART0_Input_Unsigned_Hexadecimal(void)
{
    uint32_t number = 0;
    uint32_t digit = 0;
    uint32_t length = 0;
    char character = UART0_Input_Character();

    while(character != UART0_CR)
    {
        digit = 0x10;

        if ((character >= '0') && (character <= '9'))
        {
            digit = character - '0';
        }
        else if ((character >= 'A') && (character <= 'F'))
        {
            digit = (character - 'A') + 0xA;
        }
        else if ((character >= 'a') && (character <= 'f'))
        {
            digit = (character - 'a') + 0xA;
        }

        if (digit <= 0xF)
        {
            number = (number * 0x10) + digit;
            length++;
            UART0_Output_Character(character);
        }
        else if ((character == UART0_BS) && length)
        {
            number /= 0x10;
            length--;
            UART0_Output_Character(character);
        }

        character = UART0_Input_Character();
    }

    return number;
}

void UART0_Output_Unsigned_Hexadecimal(uint32_t number)
{
    if (number >= 0x10)
    {
        UART0_Output_Unsigned_Hexadecimal(number / 0x10);
        UART0_Output_Unsigned_Hexadecimal(number % 0x10);
    }
    else
    {
        if (number < 0xA)
        {
            UART0_Output_Character(number + '0');
        }
        else
        {
            UART0_Output_Character((number - 0x0A) + 'A');
        }
    }
}
void UART0_Output_Newline(void)
{
    UART0_Output_Character(UART0_CR);
    UART0_Output_Character(UART0_LF);
}

