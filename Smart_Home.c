
#include "Smart_Home.h"

static char recievedString[20] = {0};
static char instruction[20] = {0};
static char operator[20] = {0};

extern void Smart_Home_Init ()
{
	DIO_Init();
	UART_Init();
	TIMER1_OCR1A_WRITE(TIMER1_TOP_VALUE);
	SERVO_setAngle(0);
	TIMER1_Init(TIMER1_PRESCALER_8, TIMER1_FAST_PWM_OCR1A_TOP, TIMER1_OC1A_DISCONNECTED, TIMER1_OC1B_CLEAR_ON_COMPARE_MATCH);
	
	LCD_Init();
	
	SEI ();
	UART_RecieverEnable();
	
	UART_recieveString_Asyncronous (recievedString);
}

extern void Smart_Home_Run ()
{
	Smart_Home_stringAnalysis ();
	
	Smart_Home ();
}

static void Smart_Home_stringAnalysis ()
{
	u8 recievedStringIndex = 0;
	u8 instructionIndex = 0;
	u8 operatorIndex = 0;
	
	while (recievedString[recievedStringIndex] && recievedString[recievedStringIndex] != ' '  && recievedStringIndex < 18)
	{
		instruction[instructionIndex] = recievedString[recievedStringIndex];
		instructionIndex++;
		recievedStringIndex++;
	}
	instruction[instructionIndex] = NULL;
	
	recievedStringIndex++;
	while (recievedString[recievedStringIndex] && operatorIndex < 19)
	{
		operator[operatorIndex] = recievedString[recievedStringIndex];
		operatorIndex++;
		recievedStringIndex++;
	}
	if (operator[operatorIndex - 1] == 13)
	{
		operator[operatorIndex - 1] = NULL;
	}
}

static s32 Smart_Home_operatorStringToNumber ()
{
	s32 number = 0;
	u8 signFlag = 0;
	
	if (operator[0] == '-')
	{
		signFlag = 1;
	}
	else
	{
		number += operator[0] - '0';
	}
	
	for (u8 index = 1; operator[index]; index++)
	{
		number = number * 10 + ( operator[index] - '0' );
	}
	
	if (signFlag == 1)
	{
		number = -number;
	}
	
	return number;
}

static void Smart_Home_instructionDone ()
{
	for (u8 index = 0; index < 20; index++)
	{
		recievedString[index] = NULL;
		instruction[index] = NULL;
		operator[index] = NULL;
	}
	
	UART_recieveString_Asyncronous (recievedString);
}

static u8 Smart_Home_searchEnter ()
{
	for (u8 index = 0; index < 20; index++)
	{
		if (recievedString[index] == 13)
		{
			return 1;
		}
	}
	
	return 0;
}

static void Smart_Home ()
{
	if (Smart_Home_searchEnter())
	{
		if (Smart_Home_instructionCompare("lcd"))
		{
			LCD_Clear();
			LCD_GoTo(0, 0);
			LCD_WriteString(operator);
		}
		else if (Smart_Home_instructionCompare("servo"))
		{
			SERVO_setAngle(Smart_Home_operatorStringToNumber ());
		}
		else if (Smart_Home_instructionCompare("ledon"))
		{
			switch (Smart_Home_operatorStringToNumber())
			{
				case 1:
				LED1_On();
				break;
				case 2:
				LED2_On();
				break;
				case 3:
				LED3_On();
				break;
			}
		}
		else if (Smart_Home_instructionCompare("ledoff"))
		{
			switch (Smart_Home_operatorStringToNumber())
			{
				case 1:
				LED1_Off();
				break;
				case 2:
				LED2_Off();
				break;
				case 3:
				LED3_Off();
				break;
			}
		}
		
		Smart_Home_instructionDone();
	}
}

static	u8 Smart_Home_instructionCompare (const char constant_Instuction[])
{	
	for (u8 index = 0; instruction[index] || constant_Instuction[index]; index++)
	{
		if ((instruction[index] != constant_Instuction[index]) && (instruction[index] != 'A' + (constant_Instuction[index] - 'a')) && (instruction[index] != 'a' + (constant_Instuction[index] - 'A')))
		{
			return 0;
		}
	}
	
	return 1;
}