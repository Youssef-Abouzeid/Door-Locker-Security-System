#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"
#include "gpio.h"
/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define BUZZER_PORT_ID PORTD_ID
#define BUZZER_PIN_ID  PIN2_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Setup the direction for the buzzer pin as output pin through the GPIO driver.
 * Turn off the buzzer through the GPIO.
 * Inputs: None
 * Return: None
 */
void Buzzer_init(void);

/*
 * Description :
 * Function to enable the Buzzer through the GPIO.
 * Inputs: None
 * Return: None
 */
void Buzzer_on(void);

/*
 * Description :
 * Function to disable the Buzzer through the GPIO.
 * Inputs: None
 * Return: No

 */
void Buzzer_off(void);



#endif /* BUZZER_H_ */
