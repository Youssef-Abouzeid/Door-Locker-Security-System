#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/*#define WITHOUT_PWM_MODE*/
#define NUMBER_OF_DCMOTOR 1

#if (NUMBER_OF_DCMOTOR == 1)
#define DCMOTOR_INPUT1_PORT PORTB_ID
#define DCMOTOR_INPUT1_PIN  PIN0_ID

#define DCMOTOR_INPUT2_PORT PORTB_ID
#define DCMOTOR_INPUT2_PIN  PIN1_ID

#define DCMOTOR_ENABLE1_PORT PORTB_ID
#define DCMOTOR_ENABLE1_PIN  PIN3_ID

#else

#define DCMOTOR_INPUT3_PORT
#define DCMOTOR_INPUT3_PIN

#define DCMOTOR_INPUT4_PORT
#define DCMOTOR_INPUT4_PIN



#define DCMOTOR_ENABLE2_PORT
#define DCMOTOR_ENABLE2_PIN

#endif
/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum{

	CW,A_CW,STOP

}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 * */
void DcMotor_init(void);

/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value.
 * */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);



#endif /* DC_MOTOR_H_ */
