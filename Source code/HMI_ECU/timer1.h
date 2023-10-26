#ifndef TIMER1_H_
#define TIMER1_H_

#include"std_types.h"

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum{

	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024

}Timer1_Prescaler;

typedef enum{

	NORMAL,COMPARE=4

}Timer1_Mode;

typedef struct{

	uint16 initial_value;
	uint16 compare_value; /* It will be used in Compare mode only*/
	Timer1_Prescaler prescaler;
	Timer1_Mode mode;

}Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function to initialize the Timer driver
 * Inputs: pointer to the configuration structure with type Timer1_ConfigType.
 * Return: None
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);


/*
 * Description :
 * Function to disable the Timer1.
 * Inputs: None
 * Return: None
 */
void Timer1_deInit(void);

/*
 * Description :
 * Function to set the Call Back function address.
 * Inputs: pointer to Call Back function.
 * Return: None
 */
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER1_H_ */
