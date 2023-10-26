#include "pwm.h"
#include "common_macros.h"
#include <avr/io.h>



void PWM_Timer0_Start (uint8 duty_cycle){

	/* TCCR0 Register Bits Description:
	 * WGM01:0 = 11 to choose Fast PWM
	 * COM01:0 = 10 to choose non-inverting mode
	 * CS02:0: = 010 to choose TIMER0_CLOCK = F_CPU/64, so PWM Signal = 500Hz
	 * TCNTO = 0 start TIMER0
	 * OCR0 = Duty_cycle to set duty cycle according to the input value
	 */

	TCCR0 = (1 << WGM00) | (1 << WGM01) |(1 << COM01) |(1 << CS01)| (1 << CS00);
	TCNT0 = 0;

	switch(duty_cycle){

	case 0 :
		duty_cycle = 0;
		break;

	case 25:
		duty_cycle = 64;
		break;

	case 50:
		duty_cycle = 128;
		break;

	case 75:
		duty_cycle = 192;
		break;

	case 100:
		duty_cycle = 255;
		break;

	}

	OCR0 = duty_cycle;



}
