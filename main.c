/*
 * main.c
 *
 *  Created on: May 5, 2022
 *      Author: Yasser
 */

/**************************************************Includes****************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/**********************************************Symbolic Constants**********************************************/
#define SEVEN_SEGMENTS_SELECT_OUTPUT 0x3F
#define SEVEN_SEGMENTS_DECODER_OUTPUT 0x0F
#define ONE_SECOND 1000

/*************************************************Prototypes**************************************************/
void writeTime(unsigned char hour,unsigned char min, unsigned char sec);
void Timer1_Init_CTC_Mode(unsigned int tick);
void INT0_Init(void);
void INT1_Init(void);
void INT2_Init(void);
/**********************************************Global Variables***********************************************/
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;

/***********************************************Main Function**************************************************/
int main(void){
	DDRA |= SEVEN_SEGMENTS_SELECT_OUTPUT;
	DDRC |= SEVEN_SEGMENTS_DECODER_OUTPUT;
	Timer1_Init_CTC_Mode(ONE_SECOND);
	SREG |= (1<<7);    //Enable global interrupt
	INT0_Init();
	INT1_Init();
	INT2_Init();
	while(1){
		writeTime(hours, minutes, seconds);
	}
}

/***********************************************writeTime Function**************************************************/

/*
 * This Function takes hours, minutes, and seconds as parameters,
 * extracts their digits,
 * and write each digit on a seven segment in a round robin fashion
 */
void writeTime(unsigned char hour,unsigned char min, unsigned char sec){
	unsigned char sec1, sec2;
	unsigned char min1, min2;
	unsigned char hour1, hour2;
	unsigned char num;

	sec1 = sec % 10; 		//first digit of seconds
	sec = sec / 10;
	sec2 = sec; 			//second digit of seconds

	min1 = min % 10; 		//first digit of minutes
	min = min / 10;
	min2 = min; 			//second digit of minutes

	hour1 = hour % 10; 		//first digit of hours
	hour = hour / 10;
	hour2 = hour; 			//second digit of hours

	PORTA = (1<<0); 				//enable first 7-segment
	num = sec1;
	PORTC = (PORTC & 0xF0) | (num & 0x0F);
	_delay_us(20);

	PORTA = (1<<1); 				//enable second 7-segment
	num = sec2;
	PORTC = (PORTC & 0xF0) | (num & 0x0F);
	_delay_us(20);

	PORTA = (1<<2); 				//enable third 7-segment
	num = min1;
	PORTC = (PORTC & 0xF0) | (num & 0x0F);
	_delay_us(20);

	PORTA = (1<<3); 				//enable fourth 7-segment
	num = min2;
	PORTC = (PORTC & 0xF0) | (num & 0x0F);
	_delay_us(20);
	PORTA = (1<<4); 				//enable fifth 7-segment
	num = hour1;
	PORTC = (PORTC & 0xF0) | (num & 0x0F);
	_delay_us(20);

	PORTA = (1<<5); 				//enable sixth 7-segment
	num = hour2;
	PORTC = (PORTC & 0xF0) | (num & 0x0F);
	_delay_us(20);
}

/*********************************************Timer1 Initialization********************************************/
void Timer1_Init_CTC_Mode(unsigned int tick)
{
	TCNT1 = 0;    			// Set Timer initial value to 0
	OCR1A  = tick; 			// Set Compare Value
	TIMSK |= (1<<OCIE1A); 	// Enable Timer1A Compare Interrupt
	TCCR1A = (1<<FOC1A); 	//CTC Mode, TOP OCR1A
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10); //CTC Mode, clock = F_CPU/1024
}

/*********************************************INT0 Initialization********************************************/
void INT0_Init(void)
{
	SREG  &= ~(1<<7);                   // Disable interrupts by clearing I-bit
	DDRD  &= (~(1<<PD2));               // Configure INT0/PD2 as input pin
	PORTD |= (1 << PD2);				//Enable internal pull up resistor
	MCUCR |= (1<<ISC01);   				// Trigger INT0 with the falling edge
	GICR  |= (1<<INT0);                 // Enable external interrupt pin INT0
	SREG  |= (1<<7);                    // Enable interrupts by setting I-bit
}

/*********************************************INT1 Initialization********************************************/
void INT1_Init(void)
{
	SREG  &= ~(1<<7);      				// Disable interrupts by clearing I-bit
	DDRD  &= ~(1<<PD3);  				// Configure INT1/PD3 as input pin
	MCUCR |= (1<<ISC10) | (1<<ISC11); 	// Trigger INT1 with the rising edge
	GICR  |= (1<<INT1);    				// Enable external interrupt pin INT1
	SREG  |= (1<<7);       				// Enable interrupts by setting I-bit
}

/*********************************************INT2 Initialization********************************************/
void INT2_Init(void)
{
	SREG   &= ~(1<<7);                  // Disable interrupts by clearing I-bit
	DDRB   &= ~(1<<PB2);               	// Configure INT2/PB2 as input pin
	PORTB  |= (1 << PB2);				//Enable pull up resistor
	MCUCSR &= ~(1<<ISC2);   			// Trigger INT2 with the falling edge
	GICR   |= (1<<INT2);                // Enable external interrupt pin INT2
	SREG   |= (1<<7);                   // Enable interrupts by setting I-bit
}

/************************************Interrupt Service Routine Of Timer1***************************************/
ISR(TIMER1_COMPA_vect)
{
	/*reset the stop watch at 99:59:59*/
	if(hours >= 99 && minutes >= 59 && seconds >= 59){
		hours = 0;
		minutes = 0;
		seconds = 0;
	}

	/*
	 * increment seconds if less than 59, otherwise reset seconds and check minutes
	 * increment minutes if less than 59, otherwise reset minutes and increment hours
	 */
	if(seconds >= 59){
		seconds = 0;
		if(minutes >= 59){
			minutes = 0;
			hours++;
		}

		else{
			minutes++;
		}
	}

	else{
		seconds++;
	}
}

/*****************************Interrupt Service Routine Of INT0 (Stopwatch Reset)******************************/
ISR(INT0_vect)
{
	hours = 0;
	minutes = 0;
	seconds = 0;
}

/*****************************Interrupt Service Routine Of INT1 (Stopwatch Pause)******************************/

ISR(INT1_vect)
{
	TCCR1B &= ~(1 << CS12) & ~(1<<CS10); //disable timer clock
}

/*****************************Interrupt Service Routine Of INT2 (Stopwatch Resume)******************************/
ISR(INT2_vect)
{
	TCCR1B |= (1<<CS12) | (1<<CS10); //clock = F_CPU/1024
}



