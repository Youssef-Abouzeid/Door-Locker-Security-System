#include"uart.h"
#include"timer1.h"
#include"twi.h"
#include"external_eeprom.h"
#include"Dc_motor.h"
#include"buzzer.h"
#include<util/delay.h>
#include<avr/io.h>

/*******************************************************************************
 *                              MACROS                                         *
 *******************************************************************************/
#define PASS_READY 			1
#define PASS_CHECK 			2
#define PASS_MATCH 			3
#define OPEN_DOOR  			4
#define MAX_DIGITS 			5
#define NO_MATCH   			12
#define SYSTEM_LOCKED       14
#define UNLOCKING_TIME 		15
#define STOP_TIME           18
#define LOCKING_TIME   		33



/*******************************************************************************
 *                            Global Variables                                 *
 *******************************************************************************/
/* Global variables to store the received passwords */
static uint32 g_pass,g_checkpass,g_eeprompass;

/* Global variable to count the time */
static uint8 g_count;

/*******************************************************************************
 *                            Functions Prototypes                             *
 *******************************************************************************/
void Timer();	/* Timing function to count the elapsed time */
void Get_pass(void);	/* Store the received password in g_pass*/
void Check_pass(void);	/* Store the received password in g_checkpass and g_eeprompass only if the g_pass == g_checkpass */
void Open_Door(void);	/* Open the door if received password == g_eeprompass*/

/*******************************************************************************
 *                            Application Layer                                *
 *******************************************************************************/


int main(void){

	/* Enable Global interrupt */
	SREG |= (1<<7);

	/* Initialize the UART Driver */
	UART_ConfigType myconfig ={Eight,Disabled,One,9600};
	UART_init(&myconfig);

	/* Initialize the TIMER1 Driver */
	Timer1_ConfigType timerconfig = {0,31250,F_CPU_256,COMPARE};
	Timer1_init(&timerconfig);
	Timer1_setCallBack(Timer);

	/* Initialize the TWI/I2C Driver */
	TWI_ConfigType twiconfig= {Slave_1,Fast_mode};
	TWI_init(&twiconfig);

	/* Initialize the DC-MOTOR Driver */
	DcMotor_init();

	/* Initialize the BUZZER Driver */
	Buzzer_init();


	while(1){

		switch(UART_recieveByte()){ /* Receive byte from HMI_ECU */

		case PASS_READY: /* Received byte from HMI_ECU == PASS_READY */
			Get_pass();
			break;
		case PASS_CHECK:	/* Received byte from HMI_ECU == PASS_CHECK */
			Check_pass();
			break;
		case OPEN_DOOR:	/* Received byte from HMI_ECU == OPEM_DOOR */
			Open_Door();
			break;
		default:	/* If no matching case exist the loop */
			break;

		}

	}
}


/*******************************************************************************
 *                             Functions Definitions                           *
 *******************************************************************************/

void Timer(void){
	g_count++;	/* Increment the counter each second */

}


void Get_pass(void){

	uint8 pass[MAX_DIGITS]={0};	/* Create array of 5 Digits to store received keys*/
	uint8 i=0;	/* Counter for each pressed key */
	g_pass=0;	/* Reset g_pass each function call */

	while(i<MAX_DIGITS){
		pass[i] = UART_recieveByte();
		g_pass = g_pass*10 + pass[i];	/* Store keys in combined form to create the password*/
		i++;
	}
	while(UART_recieveByte()!='#');	/* Wait for the Enter key from HMI_ECU */


}

void Check_pass(void){

	uint8 pass[MAX_DIGITS]={0}; /* Create array of 5 Digits to store received keys*/
	uint8 i=0;	/* Counter for each pressed key */
	g_checkpass=0;	/* Reset g_checkpass each function call */
	static uint8 sec_count;	/* Security counter */

	while(i<MAX_DIGITS){
		pass[i] = UART_recieveByte();
		g_checkpass = g_checkpass*10 + pass[i];	/* Store keys in combined form to create the password*/
		i++;
	}
	while(UART_recieveByte()!='#');	/* Wait for the Enter key from HMI_ECU */

	if(g_checkpass == g_eeprompass ){ /* Check if the two passwords are matching */
		sec_count=0; /* Reset the security counter */
		UART_sendByte(PASS_MATCH); /* Send the byte to Change_pass function in HMI_ECU */
	}
	else if(g_pass == g_checkpass){ /* Check if the two passwords are matching */
		sec_count=0; /* Reset the security counter */
		EEPROM_writeByte(0x01,&g_pass,4); /* Write the password in eeporm memomry */
		_delay_ms(10);
		EEPROM_readByte(0x01,&g_eeprompass,4);	/* Read the stored password from eeprom memory */
		_delay_ms(10);
		UART_sendByte(PASS_MATCH);/* Send the byte to Check_pass function in HMI_ECU */

	}

	else{
		sec_count++;	/* Increment the security counter if no matching passwords */
		if(sec_count == 3){	/* User exceeded number of tries */
			sec_count=0;	/* Reset the security counter */
			g_count=0;	/* Reset the timer, To count Activation time of the buzzer*/
			UART_sendByte(SYSTEM_LOCKED); 	/* Send the byte to Check_pass function in HMI_ECU */
			Buzzer_on(); /*	Activate the buzzer */
			while(g_count<=60);	/* Activation time: 60 seconds */
			Buzzer_off();	/*Deactivate the buzzer */
		}
		UART_sendByte(NO_MATCH);	/* Send the byte to Check_pass function in HMI_ECU */
	}

}

void Open_Door(void){

	uint8 pass[MAX_DIGITS]={0};	/* Create array of 5 Digits to store received keys*/
	uint8 i=0;	/* Counter for each pressed key */
	uint16 pass_check=0;	/* Reset pass_check each function call */
	static uint8 sec_count;	/* Security counter */

	while(i<MAX_DIGITS){
		pass[i] = UART_recieveByte();
		pass_check = pass_check*10 + pass[i];	/* Store keys in combined form to create the password*/
		i++;
	}
	while(UART_recieveByte()!='#');	/* Wait for the Enter key from HMI_ECU */

	if(pass_check == g_eeprompass){	/* Check if the two passwords are matching */

		g_count=0;	/* Reset the timer, To count Operation of DC-MOTOR */
		sec_count=0;	/* Reset the security counter */
		UART_sendByte(PASS_MATCH);

		DcMotor_Rotate(CW,100);	/* Rotate the DC-MOTOR ClockWise with Max speed */
		while(g_count <= UNLOCKING_TIME); /* Rotation time: 15sec */
		DcMotor_Rotate(STOP,0);	/* Stop the DC-MOTOR */
		while(g_count <= STOP_TIME);	/*Stop time: 3sec */
		DcMotor_Rotate(A_CW,100);	/* Rotate the DC-MOTOR Anti-ClockWise with Max speed */
		while(g_count <= LOCKING_TIME);	/* Rotation time: 15sec */
		DcMotor_Rotate(STOP,0);	/* Stop the DC-MOTOR */
		return ;	/* Exist the function */
	}
	else{
		sec_count++;	/* Increment the security counter if no matching passwords */
		if(sec_count == 3){	/* User exceeded number of tries */
			sec_count=0;	/* Reset the security counter */
			g_count=0;	/* Reset the timer, To count Activation time of the buzzer*/
			UART_sendByte(SYSTEM_LOCKED);	/* Send the byte to Check_pass function in HMI_ECU */
			Buzzer_on();	/*	Activate the buzzer */
			while(g_count<=60);	/* Activation time: 60 seconds */
			Buzzer_off();	/*Deactivate the buzzer */
		}
		UART_sendByte(NO_MATCH);	/* Send the byte to Check_pass function in HMI_ECU */
	}

}

