#include"lcd.h"
#include"keypad.h"
#include"uart.h"
#include"timer1.h"
#include<util/delay.h>
#include<avr/io.h>

/*******************************************************************************
 *                              MACROS                                         *
 *******************************************************************************/
#define PASS_READY 		1
#define PASS_CHECK		2
#define PASS_MATCH 		3
#define OPEN_DOOR  		4
#define MAX_DIGITS 		5
#define NO_MATCH   		12
#define ENTER      		13
#define SYSTEM_LOCKED   14
#define UNLOCKING_TIME 	18
#define LOCKING_TIME   	33

/*******************************************************************************
 *                            Global Variables                                 *
 *******************************************************************************/
/* Global variable to count the time */
static uint8 g_count;


/*******************************************************************************
 *                            Functions Prototypes                             *
 *******************************************************************************/
void Timer(void);	/* Timing function to count the elapsed time */
void Create_pass(void);	/* Create the initial password for the system */
void Check_pass(void);	/* Check if the two passwords are matching */
void Open_Door(void);	/* Open the door if the correct password is entered */
void Change_Pass(void);	/* Enter the current password then enter the new password */
void Theft_alert(void);	/* Activate the Security system if the provided password is incorrect 3 times */



/*******************************************************************************
 *                            Application Layer                                *
 *******************************************************************************/
int main(void)
{
	/* Get the key pressed from the keypad*/
	volatile uint8 key;

	/* Enable Global interrupt */
	SREG |= (1<<7);

	/* Initialize the TIMER1 Driver */
	Timer1_ConfigType timerconfig = {0,31250,F_CPU_256,COMPARE};
	Timer1_init(&timerconfig);
	Timer1_setCallBack(Timer);

	/* Initialize the UART Driver */
	UART_ConfigType myconfig ={Eight,Disabled,One,9600};
	UART_init(&myconfig);

	/* Initialize the LCD driver */
	LCD_init();

	/* Get the matching password from the user*/
	do {
		Create_pass();
		Check_pass();
	} while (UART_recieveByte()!= PASS_MATCH);	/* Don't exit until the user enter a matching password*/


	while(1){

		/* Main Options */
		LCD_displayStringRowColumn(0,0,"+ : Open Door");
		LCD_displayStringRowColumn(1,0,"- : Change Pass");

		/* Get the key pressed from the keypad*/
		key = KEYPAD_getPressedKey();

		switch (key) {

		case '+':	/* If User input from keypad: '+', Activate Open Door option */
			Open_Door();
			break;
		case '-':/* If User input from keypad: '-', Activate Change pass option */
			Change_Pass();
			break;
		default: /* If no matching case exist the loop */
			break;

		}

	}


}

/*******************************************************************************
 *                             Functions Definitions                           *
 *******************************************************************************/

void Timer(void)
{
	g_count++;	/* Increment the counter each second */

}

void Create_pass(void)
{
	/* Get the key pressed from the keypad*/
	volatile uint8 key;
	uint8 i = 0;	/* Counter for each pressed key */

	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	LCD_moveCursor(1,0);
	UART_sendByte(PASS_READY);	/* Call get_pass function in Control_ECU */
	while(1)
	{
		key = KEYPAD_getPressedKey();
		if(key>=0 && key<=9 && i< MAX_DIGITS )
		{
			UART_sendByte(key);	/* Send each pressed Key to get_pass function */
			LCD_displayCharacter('*');	/* Display '*' for each pressed key */
			i++;

		}

		if(key == ENTER)	/* User entered specified Enter key  */
		{
			LCD_clearScreen(); /* Clear LCD screen each successful operation */
			UART_sendByte('#');	/* Tell Control_ECU to terminate get_pass function */
			break;
		}
		_delay_ms(550); /* Get pressed key each 550ms */

	}
}

void Check_pass(void)
{
	/* Get the key pressed from the keypad*/
	volatile uint8 key;
	uint8 i = 0;	/* Counter for each pressed key */

	LCD_displayString("plz re-enter the");
	LCD_displayStringRowColumn(1,0,"same pass: ");
	UART_sendByte(PASS_CHECK);	/* Call Check_pass function in Control_ECU */

	while(1)
	{
		key = KEYPAD_getPressedKey();

		if(key>=0 && key<=9 && i < MAX_DIGITS )
		{
			UART_sendByte(key);	/* Send each pressed Key to get_pass function */
			LCD_displayCharacter('*');	/* Display '*' for each pressed key */
			i++;
		}

		if(key == ENTER)	/* User entered specified Enter key  */
		{
			LCD_clearScreen();	/* Clear LCD screen each successful operation */
			UART_sendByte('#');	/* Tell Control_ECU to terminate Check_pass function */
			return ;	/* Exist the function upon completion*/
		}

		_delay_ms(550); /* Get pressed key each 550ms */
	}

}

void Open_Door(void)
{
	/* Get the key pressed from the keypad*/
	volatile uint8 key;
	uint8 i = 0;	/* Counter for each pressed key */

	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	LCD_moveCursor(1,0);
	UART_sendByte(OPEN_DOOR);	/* Call Open_Door function in Control_ECU */

	while(1)
	{
		key = KEYPAD_getPressedKey();

		if(key>=0 && key<=9 && i<MAX_DIGITS )
		{
			UART_sendByte(key);	/* Send each pressed Key to Check_pass function */
			LCD_displayCharacter('*');	/* Display '*' for each pressed key */
			i++;

		}

		if(key == ENTER)	/* User entered specified Enter key  */
		{
			LCD_clearScreen();	/* Clear LCD screen each successful operation */
			UART_sendByte('#');	/* Tell Control_ECU to terminate Open_Door function */

			switch( UART_recieveByte() ){ 	/* Receive byte from Control_ECU */

			case NO_MATCH :	/* If input password != Stored password in eeporm */
				i = 0;
				LCD_displayString("plz enter pass:");
				LCD_moveCursor(1,0);
				UART_sendByte(OPEN_DOOR);	/* Recall Open_Door function n Control_ECU */
				break; /* Break from the loop, To re-enter the password */

			case PASS_MATCH : /* If input password == Stored password in eeporm */

				g_count=0;	/*	Reset the timer, To count the Displayed message*/
				LCD_clearScreen();
				LCD_displayString("Door: Unlocking");
				while(g_count <= UNLOCKING_TIME);	/* Wait until the g_count reaches the end of Unlocking_time */
				LCD_clearScreen();
				LCD_displayString("Door: locking");	/* Wait until the g_count reaches the end of locking_time */
				while(g_count <= LOCKING_TIME);
				return; /* Exist from the function and return to the Main options */

			case SYSTEM_LOCKED:	/* If the User input the wrong password 3-times in a row */
				Theft_alert(); /* Activate the Security system */
				return;	/* Exist from the function and return to the Main options */

			}

		}


		_delay_ms(550);	/* Get pressed key each 550ms */
	}


}

void Change_Pass(void)
{
	/* Get the key pressed from the keypad*/
	volatile uint8 key;
	uint8 i = 0;	/* Counter for each pressed key */

	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	LCD_moveCursor(1,0);
	UART_sendByte(PASS_CHECK);	/* Call Check_pass function in Control_ECU */

	while(1)
	{
		key = KEYPAD_getPressedKey();

		if(key>=0 && key<=9 && i<MAX_DIGITS )
		{
			UART_sendByte(key);	/* Send each pressed Key to Check_pass function */
			LCD_displayCharacter('*');	/* Display '*' for each pressed key */
			i++;

		}

		if(key == ENTER)	/* User entered specified Enter key  */
		{
			LCD_clearScreen();	/* Clear LCD screen each successful operation */
			UART_sendByte('#');	/* Tell Control_ECU to terminate Check_pass function */

			switch( UART_recieveByte() ){	/* Receive byte from Control_ECU */

			case NO_MATCH :	/* If input password != Stored password in eeporm */
				i = 0;
				LCD_displayString("plz enter pass:");
				LCD_moveCursor(1,0);
				UART_sendByte(PASS_CHECK);	/* Recall Check_pass function n Control_ECU */
				break;	/* Break from the loop, To re-enter the password */

			case PASS_MATCH :	/* If input password == Stored password in eeporm */
				/* Get the matching password from the user */
				do {
					_delay_ms(100);
					Create_pass();
					Check_pass();
				} while (UART_recieveByte()!= PASS_MATCH);	/* Don't exit until the user enter a matching password*/
				return;

			case SYSTEM_LOCKED:	/* If the User input the wrong password 3-times in a row */
				Theft_alert();	/* Activate the Security system */
				return;	/* Exist from the function and return to the Main options */

			}

		}

		_delay_ms(550);	/* Get pressed key each 550ms */
	}

}

void Theft_alert(void){
	g_count = 0;	/*	Reset the timer, To count the Displayed message */
	LCD_clearScreen();
	LCD_displayString("Security System:");
	LCD_displayStringRowColumn(1,0,"Locked.");
	while(g_count<=60);	/* Display the message for 60 seconds */
	LCD_clearScreen();	/* Clear LCD screen before existing the function */

}


