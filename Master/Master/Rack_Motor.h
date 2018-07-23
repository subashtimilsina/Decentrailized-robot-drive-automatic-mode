/* 
* Rack_Motor.h
*
* Created: 5/25/2018 5:03:48 PM
* Author: Subash Timilsina
*/


#ifndef __RACK_MOTOR_H__
#define __RACK_MOTOR_H__


#include "headers.h"

#define RACK_ICR_TOP   249
#define RACK_MAX_VALUE 249
#define RACK_MIN_VALUE -249




//For rack motor


#define DD_F5 A,1
#define DD_B5 A,3

#define DD_PWM5			L,5
#define PWM_TCCRA5		TCCR5A
#define PWM_TCCRB5		TCCR5B
#define PWM_ICR5			ICR5
#define PWM_OCR5			OCR5C

#define PWM_5COM0		COM5C0
#define PWM_5COM1		COM5C1

#define PWM_5WGM0		WGM50
#define PWM_5WGM1		WGM51
#define PWM_5WGM2		WGM52
#define PWM_5WGM3		WGM53
#define PWM_5CS0			CS50
#define PWM_5CS1			CS51
#define PWM_5CS2			CS52

//For geneva motor

#define DD_F6		A,7
#define DD_B6		A,5

#define DD_PWM6			L,4
#define PWM_TCCRA6		TCCR5A
#define PWM_TCCRB6		TCCR5B
#define PWM_ICR6			ICR5
#define PWM_OCR6			OCR5B

#define PWM_6COM0		COM5B0
#define PWM_6COM1		COM5B1

#define PWM_6WGM0		WGM50
#define PWM_6WGM1		WGM51
#define PWM_6WGM2		WGM52
#define PWM_6WGM3		WGM53
#define PWM_6CS0			CS50
#define PWM_6CS1			CS51
#define PWM_6CS2			CS52


class Rack_Motor
{

	private:
	uint8_t num;
	public:
	void Initialise(uint8_t no);

	void InitPWM();

	void SetForwardDirection();
	void SetReverseDirection();
	void StopMotor();
	void SetOcrValue(int x);
};

#endif //__RACK_MOTOR_H__
