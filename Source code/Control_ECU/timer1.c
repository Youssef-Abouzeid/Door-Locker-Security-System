#include"timer1.h"
#include<avr/io.h> /* To use Timer1 Registers*/
#include<avr/interrupt.h> /* For Timer ISR */


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
static volatile void (*g_callBackPtr)(void) = NULL_PTR;


/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER1_OVF_vect){

	if(g_callBackPtr != NULL_PTR){
		/* Call the Call Back function in the application after Overflow */
		(*g_callBackPtr)();
	}
}


ISR(TIMER1_COMPA_vect){

	if(g_callBackPtr != NULL_PTR){
			/* Call the Call Back function in the application after Compare Match */
			(*g_callBackPtr)();
		}
}


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Function to initialize the Timer driver
 * Inputs: pointer to the configuration structure with type Timer1_ConfigType.
 * Return: None
 */

void Timer1_init(const Timer1_ConfigType * Config_Ptr){

	/************************** TCCR1A Description **************************
	 * COM1A1:0 = 0 Disable Compare Output Mode for Compare unit A
	 * COM1B1:0 = 0 Disable Compare Output Mode for Compare unit B
	 * FOC1A = 1    Enable Force Output Compare for Compare unit A
	 * FOC1B = 1    Enable Force Output Compare for Compare unit B
	 * WGM11:10     Waveform Generation Mode
	 ***********************************************************************/
	TCCR1A = (1<<FOC1A) |(1<<FOC1B);
	TCCR1A = (( Config_Ptr->mode & 0x02) << WGM10);

	/************************** TCCR1A Description **************************
	 * ICNC1 = 0 Disable Input Capture Noise Canceler
	 * ICES1 = 0 Disable Input Capture Edge Select
	 * Reserved Bit
	 * WGM13:2 : Waveform Generation Mode
	 * CS12:0  : Clock Select
	 ***********************************************************************/
	TCCR1B = ( (Config_Ptr->mode & 0x0C) <<WGM12) | (Config_Ptr->prescaler <<CS10);


	/************************** TCNT1 Description **************************
	 * TCNT1 give direct access, both for read and for write operations,
	 * to the Timer/Counter unit 16-bit counter.
	 ***********************************************************************/
	TCNT1 = Config_Ptr->initial_value;

	/************************** OCR1A:B Description **************************
	 * The Output Compare Registers contain a 16-bit value that is continuously
	 * compared with the counter value (TCNT1).
	 * A match can be used to generate an output compare interrupt,
	 * or to generate a waveform output on the OC1x pin.
	 ***********************************************************************/
	OCR1A = Config_Ptr->compare_value;

	/************************** TIMSK Description **************************
	 * TICIE1 = 0 Disable Input Capture Interrupt Enable
	 * OCIE1A = 1 Enable Output Compare A Match Interrupt Enable
	 * OCIE1B = 0 Disable Output Compare B Match Interrupt Enable
	 * TOIE1  = 1 Enable  Overflow Interrupt Enable
	 ***********************************************************************/
	TIMSK |= (1<<OCIE1A) | (1<<TOIE1);


}

/*
 * Description :
 * Function to disable the Timer1.
 * Inputs: None
 * Return: None
 */
void Timer1_deInit(void){

	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	OCR1A  = 0;

	/* Disable the Timer1 interrupt */
	TIMSK &= ~(1<<OCIE1A) & ~(1<<TOIE1);

	/* Reset the global pointer value */
	g_callBackPtr = NULL_PTR;
}

/*
 * Description :
 * Function to set the Call Back function address.
 * Inputs: pointer to Call Back function.
 * Return: None
 */
void Timer1_setCallBack(void(*a_ptr)(void)){

	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}




















