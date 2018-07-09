/* 
* PID.cpp
*
* Created: 10/28/2017 11:53:40 AM
* Author: Subash Timilsina
*/


#include "PID.h"

void PID::Set_Pid(float kP,float kI,float kD)
{
	kp = kP;
	ki = kI;
	kd = kD;	
}


float PID::Compute(int input)
{
	error = sp - input;
	
	errorsum += error;
	
	//Limiting I-term
	if (errorsum > 125)
		errorsum = 125;
	else if (errorsum < -125)
		errorsum = -125;
	
	cv += error*kp + errorsum*ki - kd*(input-lastinput);
	
	lastinput = input;
	return cv;
}

float PID::angle_Compute(int input)
{
	error = sp - input;
	
	errorsum += error;
	
	//Limiting I-term
	if (errorsum > 125)
	errorsum = 125;
	else if (errorsum < -125)
	errorsum = -125;
	
	cv = error*kp + errorsum*ki - kd*(input-lastinput);
	
	lastinput = input;
	return cv;
}
