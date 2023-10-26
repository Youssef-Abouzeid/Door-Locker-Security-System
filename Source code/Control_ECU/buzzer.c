#include "buzzer.h"



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Setup the direction for the buzzer pin as output pin through the GPIO driver.
 * Turn off the buzzer through the GPIO.
 * Inputs: None
 * Return: None
 */
void Buzzer_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID,PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}

/*
 * Description :
 * Function to enable the Buzzer through the GPIO.
 * Inputs: None
 * Return: None
 */
void Buzzer_on(void)
{

	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_HIGH);


}

/*
 * Description :
 * Function to disable the Buzzer through the GPIO.
 * Inputs: None
 * Return: No

 */
void Buzzer_off(void)
{
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);


}
