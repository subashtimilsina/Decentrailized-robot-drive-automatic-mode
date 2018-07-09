/* 
* Rack_Motor.cpp
*
* Created: 5/25/2018 5:03:48 PM
* Author: Subash Timilsina
*/


#include "Rack_Motor.h"

void Rack_Motor::Initialise(uint8_t no)
{
	num = no;
	InitPWM();

	StopMotor();
	SetOcrValue(0);

}

void Rack_Motor::InitPWM()
{
	if (num==1)
	{
		OUTPUT(DD_F5);
		OUTPUT(DD_B5);

		OUTPUT(DD_PWM5);

		PWM_TCCRA5	|=  ( 1 << PWM_5COM1 ) | ( 1 << PWM_5WGM1 );														//
		PWM_TCCRB5	|=  ( 1 << PWM_5WGM2 ) | ( 1 << PWM_5WGM3 ) | ( 1 << PWM_5CS1);									//PRESCALAR 8
		PWM_ICR5		 =        RACK_ICR_TOP;
	}
	else if(num==2)
	{
		OUTPUT(DD_F6);
		OUTPUT(DD_B6);

		OUTPUT(DD_PWM6);

		PWM_TCCRA6	|=  ( 1 << PWM_6COM1 ) | ( 1 << PWM_6WGM1 );														//
		PWM_TCCRB6	|=  ( 1 << PWM_6WGM2 ) | ( 1 << PWM_6WGM3 ) | ( 1 << PWM_6CS1);									//PRESCALAR 8
		PWM_ICR6		 =         RACK_ICR_TOP;
	}
	
	
}

void Rack_Motor::SetForwardDirection()
{
	switch(num)
	{
		case 1:
		SET		(DD_F5);
		CLEAR	(DD_B5);
		break;
		case 2:
		SET		(DD_F6);
		CLEAR	(DD_B6);
		break;
	}
	

}
void Rack_Motor::SetReverseDirection()
{
	switch(num)
	{
		case 1:
		CLEAR	(DD_F5);
		SET		(DD_B5);
		break;
		case 2:
		CLEAR	(DD_F6);
		SET		(DD_B6);
		break;
		
	}

}

void Rack_Motor::StopMotor()
{
	switch(num)
	{
		case 1:
		CLEAR	(DD_F5);
		CLEAR	(DD_B5);

		PWM_OCR5 = 0;
		break;
		case 2:
		CLEAR	(DD_F6);
		CLEAR	(DD_B6);

		PWM_OCR6 = 0;
		break;
		
	}
}


void Rack_Motor::SetOcrValue(int Ocr)
{
  if (Ocr > 0)
	SetForwardDirection();
  if( Ocr < 0)
  {
	  Ocr = -Ocr;
	  SetReverseDirection();
  }
  if( Ocr > RACK_MAX_VALUE)
	Ocr = RACK_MAX_VALUE;
	switch(num)
	{
		case 1:
		PWM_OCR5	= Ocr;
		break;
		case 2:
		PWM_OCR6	= Ocr;
		break;
	}
}