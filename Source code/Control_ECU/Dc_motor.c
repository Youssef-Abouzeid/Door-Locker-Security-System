#include"Dc_motor.h"
#include "gpio.h"
#include "pwm.h"


/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 * */

void DcMotor_init(void){

	GPIO_setupPinDirection(DCMOTOR_INPUT1_PORT,DCMOTOR_INPUT1_PIN,PIN_OUTPUT);
	GPIO_setupPinDirection(DCMOTOR_INPUT2_PORT,DCMOTOR_INPUT2_PIN,PIN_OUTPUT);
	GPIO_setupPinDirection(DCMOTOR_ENABLE1_PORT,DCMOTOR_ENABLE1_PIN,PIN_OUTPUT);

	GPIO_writePin(DCMOTOR_INPUT1_PORT,DCMOTOR_INPUT1_PIN,LOGIC_LOW);
	GPIO_writePin(DCMOTOR_INPUT2_PORT,DCMOTOR_INPUT2_PIN,LOGIC_LOW);

#ifdef WITHOUT_PWM_MODE
	GPIO_writePin(DCMOTOR_ENABLE1_PORT,DCMOTOR_ENABLE1_PIN,LOGIC_HIGH);
#endif

#if (NUMBER_OF_DCMOTOR == 2)

	GPIO_setupPinDirection(DCMOTOR_INPUT3_PORT,DCMOTOR_INPUT1_PIN,PIN_OUTPUT);
	GPIO_setupPinDirection(DCMOTOR_INPUT4_PORT,DCMOTOR_INPUT2_PIN,PIN_OUTPUT);
	GPIO_setupPinDirection(DCMOTOR_ENABLE2_PORT,DCMOTOR_ENABLE1_PIN,PIN_OUTPUT);

	GPIO_writePin(DCMOTOR_INPUT3_PORT,DCMOTOR_INPUT1_PIN,LOGIC_LOW);
	GPIO_writePin(DCMOTOR_INPUT4_PORT,DCMOTOR_INPUT2_PIN,LOGIC_LOW);

#ifdef WITHOUT_PWM_MODE
	GPIO_writePin(DCMOTOR_ENABLE1_PORT,DCMOTOR_ENABLE1_PIN,LOGIC_HIGH);
#endif

#endif

}

/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value.
 * */
void DcMotor_Rotate(DcMotor_State state,uint8 speed){




	switch(state){

	case CW:
		GPIO_writePin(DCMOTOR_INPUT1_PORT,DCMOTOR_INPUT1_PIN,LOGIC_LOW);
		GPIO_writePin(DCMOTOR_INPUT2_PORT,DCMOTOR_INPUT2_PIN,LOGIC_HIGH);
		break;

	case A_CW:
		GPIO_writePin(DCMOTOR_INPUT1_PORT,DCMOTOR_INPUT1_PIN,LOGIC_HIGH);
		GPIO_writePin(DCMOTOR_INPUT2_PORT,DCMOTOR_INPUT2_PIN,LOGIC_LOW);
		break;

	case STOP:
		GPIO_writePin(DCMOTOR_INPUT1_PORT,DCMOTOR_INPUT1_PIN,LOGIC_LOW);
		GPIO_writePin(DCMOTOR_INPUT2_PORT,DCMOTOR_INPUT2_PIN,LOGIC_LOW);
		break;

	}

	PWM_Timer0_Start (speed);

}
