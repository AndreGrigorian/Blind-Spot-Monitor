/**
 * @file main.c
 *
 * @brief Main source code for the GPIO_Interrupts program.
 *
 *This file contains the final working code for the Blind Spot Monitor
 *ECE 425 Final Project Utilizing 2x CSR04 Ultrasonic sensors to illuminate 
 *LEDS based on varying levels of danger zones. 
 *
 * @author: Andre Grigorian
 */




#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include "UART0.h"
#include <stdint.h>
#include <string.h>

// Ultrasonic Sensor Pins
// Port B
#define LEFT_TRIG      0x01  // PB0
#define LEFT_ECHO      0x02  // PB1
#define RIGHT_TRIG     0x04  // PB2
#define RIGHT_ECHO     0x08  // PB3

// LED Pins 
// Port E
#define LEFT_GREEN     0x01  // PE0
#define LEFT_YELLOW    0x02  // PE1
#define LEFT_RED       0x04  // PE2

#define RIGHT_GREEN    0x08  // PE3
#define RIGHT_YELLOW   0x10  // PE4
#define RIGHT_RED      0x20  // PE5

#define LEFT_LEDS      (LEFT_GREEN | LEFT_YELLOW | LEFT_RED)
#define RIGHT_LEDS     (RIGHT_GREEN | RIGHT_YELLOW | RIGHT_RED)
#define ALL_LEDS       (LEFT_LEDS | RIGHT_LEDS)

// Buzzer Pin
// Port A
#define BUZZER         0x04  // PA2

// Distance Thresholds
#define MAX_VALID_CM   300
//#define SAFE_CM        13 
//#define WARNING_CM     8 
//#define DANGER_CM      3 
//#define BUZZER_CLOSE_CM     2
//#define BUZZER_CRITICAL_CM  1

#define SAFE_CM        45
#define WARNING_CM     25
#define DANGER_CM      15
#define BUZZER_CLOSE_CM     10
#define BUZZER_CRITICAL_CM  5

// side selector
#define LEFT_SIDE      0
#define RIGHT_SIDE     1

//UART Top down view. Defines CM amount required to chnage distance on UART
#define DECREMENT_W 		WARNING_CM/4  
#define DECREMENT_D 		DANGER_CM/3



void GPIO_Project_Init(void);
uint32_t Read_Ultrasonic_CM(uint8_t side);
void Set_Left_LED(uint32_t distance_cm);
void Set_Right_LED(uint32_t distance_cm);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Beep_By_Distance(uint32_t distance_cm);
void UART_Render_Top_Down_View(uint32_t left_cm, uint32_t right_cm);
uint32_t Get_Closest_Danger_Distance(uint32_t left_distance, uint32_t right_distance);




int main(void)
{
    uint32_t left_distance;
    uint32_t right_distance;
    uint32_t danger_distance;

    SysTick_Delay_Init();

    GPIO_Project_Init();
    UART0_Init();


    while (1)
    {
        left_distance = Read_Ultrasonic_CM(LEFT_SIDE);
        SysTick_Delay1ms(60);

        right_distance = Read_Ultrasonic_CM(RIGHT_SIDE);
        SysTick_Delay1ms(60);


				
				UART_Render_Top_Down_View(left_distance, right_distance);
        Set_Left_LED(left_distance);
        Set_Right_LED(right_distance);

        danger_distance = Get_Closest_Danger_Distance(left_distance, right_distance);

        if (danger_distance <= DANGER_CM)
        {
            Buzzer_Beep_By_Distance(danger_distance);
        }
        else
        {
            Buzzer_Off();
        }

        SysTick_Delay1ms(100);
    }
}




void UART_Render_Top_Down_View(uint32_t left_cm, uint32_t right_cm)
{
    char left_string[10];
    char right_string[10];

    // Clear terminal and move cursor to top-left
    UART0_Output_String("\033[2J");
    UART0_Output_String("\033[H");

    // Line 1: distance values
    UART0_Output_String("Left: ");
    UART0_Output_Unsigned_Decimal(left_cm);
    UART0_Output_String(" cm   Right: ");
    UART0_Output_Unsigned_Decimal(right_cm);
    UART0_Output_String(" cm");
    UART0_Output_Newline();

    // Build left side using ranges
    if (left_cm > WARNING_CM)
    {
        strcpy(left_string, "       [");
    }
    else if (left_cm > 6)
    {
        strcpy(left_string, "*      [");
    }
    else if (left_cm > 4)
    {
        strcpy(left_string, "  *    [");
    }
    else if (left_cm > DANGER_CM)
    {
        strcpy(left_string, "    *  [");
    }
    else if (left_cm > 1)
    {
        strcpy(left_string, "     ! [");
    }
    else
    {
        strcpy(left_string, "      ![");
    }

    // Build right side using ranges
    if (right_cm > WARNING_CM)
    {
        strcpy(right_string, "]       ");
    }
    else if (right_cm > 6)
    {
        strcpy(right_string, "]      *");
    }
    else if (right_cm > 4)
    {
        strcpy(right_string, "]    *  ");
    }
    else if (right_cm > DANGER_CM)
    {
        strcpy(right_string, "]  *    ");
    }
    else if (right_cm > 1)
    {
        strcpy(right_string, "] !     ");
    }
    else
    {
        strcpy(right_string, "]!      ");
    }

    // Line 2: top down view
    UART0_Output_String(left_string);
    UART0_Output_String(right_string);
}


void GPIO_Project_Init(void)
{
    // Enable clock to Port A, Port B, and Port E
    SYSCTL->RCGCGPIO |= 0x13; // Port A = bit 0, Port B = bit 1, Port E = bit 4

    // Small delay for clock stabilization
    volatile uint32_t delay = SYSCTL->RCGCGPIO;
    delay++;

    // Port B: Ultrasonic Sensors 
    // PB0 and PB2 outputs for trigger
    GPIOB->DIR |= (LEFT_TRIG | RIGHT_TRIG);

    // PB1 and PB3 inputs for echo
    GPIOB->DIR &= ~(LEFT_ECHO | RIGHT_ECHO);

    // Disable alternate functions on PB0-PB3
    GPIOB->AFSEL &= ~(LEFT_TRIG | LEFT_ECHO | RIGHT_TRIG | RIGHT_ECHO);

    // Enable digital function on PB0-PB3
    GPIOB->DEN |= (LEFT_TRIG | LEFT_ECHO | RIGHT_TRIG | RIGHT_ECHO);

    // Start trigger pins low
    GPIOB->DATA &= ~(LEFT_TRIG | RIGHT_TRIG);

    // Port E: LEDs
    // PE0-PE5 outputs
    GPIOE->DIR |= ALL_LEDS;

    // Disable alternate functions
    GPIOE->AFSEL &= ~ALL_LEDS;

    // Enable digital function
    GPIOE->DEN |= ALL_LEDS;

    // Start all LEDs off
    GPIOE->DATA &= ~ALL_LEDS;

    // Port A: Buzzer
    // PA2 output
    GPIOA->DIR |= BUZZER;

    // Disable alternate function
    GPIOA->AFSEL &= ~BUZZER;

    // Enable digital function
    GPIOA->DEN |= BUZZER;

    // Start buzzer off
    GPIOA->DATA &= ~BUZZER;
}

uint32_t Read_Ultrasonic_CM(uint8_t side)
{
    uint8_t trig_pin;
    uint8_t echo_pin;

    uint32_t timeout;
    uint32_t pulse_us = 0;
    uint32_t distance_cm;

    if (side == LEFT_SIDE)
    {
        trig_pin = LEFT_TRIG;
        echo_pin = LEFT_ECHO;
    }
    else
    {
        trig_pin = RIGHT_TRIG;
        echo_pin = RIGHT_ECHO;
    }

    // Make sure trigger starts LOW
    GPIOB->DATA &= ~trig_pin;
    SysTick_Delay1us(2);

    // Send 10 us trigger pulse
    GPIOB->DATA |= trig_pin;
    SysTick_Delay1us(10);
    GPIOB->DATA &= ~trig_pin;

    // Wait for echo to go HIGH
    timeout = 30000;//30 ms
    while ((GPIOB->DATA & echo_pin) == 0)
    {
        SysTick_Delay1us(1);

        if (timeout == 0)
        {
            return 0;
        }

        timeout--;
    }

    // Measure how long echo stays HIGH
    timeout = 30000;
    while ((GPIOB->DATA & echo_pin) != 0)
    {
        SysTick_Delay1us(1);
        pulse_us++;

        if (timeout == 0)
        {
            return 0;
        }

        timeout--;
    }

    
    //distance_cm = pulse_us / 58;
		distance_cm = (pulse_us * 9) / 116;

    if (distance_cm > MAX_VALID_CM)
    {
        return 0;
    }

    return distance_cm ;
	
}

void Set_Left_LED(uint32_t distance_cm)
{
    // Turn off left LEDs first
    GPIOE->DATA &= ~LEFT_LEDS;

    if (distance_cm > WARNING_CM)
    {
        // Safe / no object
        GPIOE->DATA |= LEFT_GREEN;
    }
    else if (distance_cm > DANGER_CM)
    {
        // Caution
        GPIOE->DATA |= LEFT_YELLOW;
    }
    else
    {
        // Danger
        GPIOE->DATA |= LEFT_RED;
    }
}

void Set_Right_LED(uint32_t distance_cm)
{
    // Turn off right LEDs first
    GPIOE->DATA &= ~RIGHT_LEDS;

    if (distance_cm > WARNING_CM)
    {
        // Safe / no object
        GPIOE->DATA |= RIGHT_GREEN;
    }
    else if (distance_cm > DANGER_CM)
    {
        // Caution
        GPIOE->DATA |= RIGHT_YELLOW;
    }
    else
    {
        // Danger
        GPIOE->DATA |= RIGHT_RED;
    }
}

uint32_t Get_Closest_Danger_Distance(uint32_t left_distance, uint32_t right_distance)
{
    uint32_t closest = DANGER_CM+1;

    if (left_distance >= 0 && left_distance <= DANGER_CM)
    {
        closest = left_distance;
    }

    if (right_distance >= 0 && right_distance <= DANGER_CM)
    {
        if (closest == 0 || right_distance < closest)
        {
            closest = right_distance;
        }
    }

    return closest;
}

void Buzzer_Beep_By_Distance(uint32_t distance_cm)
{
	
    if (distance_cm <= BUZZER_CRITICAL_CM)
    {
        // extremely close= fastest chirp
        Buzzer_On();
        SysTick_Delay1ms(70);
        Buzzer_Off();
        SysTick_Delay1ms(5);
    }
    else if (distance_cm <= BUZZER_CLOSE_CM)
    {
        // Very close  = medium-fast chirp
        Buzzer_On();
        SysTick_Delay1ms(70);
        Buzzer_Off();
        SysTick_Delay1ms(10);
    }
    else if (distance_cm <= DANGER_CM)
    {
        // Danger zone = slower chirp
        Buzzer_On();
        SysTick_Delay1ms(70);
        Buzzer_Off();
        SysTick_Delay1ms(100);
    }
    else
    {
        Buzzer_Off();
    }
}
void Buzzer_On(void)
{
    GPIOA->DATA |= BUZZER;
}

void Buzzer_Off(void)
{
    GPIOA->DATA &= ~BUZZER;
}