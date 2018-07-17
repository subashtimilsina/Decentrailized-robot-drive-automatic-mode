/*
 * drive.h
 *
 * Created: 5/29/2018 12:57:52 PM
 *  Author: Subash Timilsina
 */ 


#ifndef DRIVE_H_
#define DRIVE_H_

#define MAX_RPM		470.0   //for 12V supply in the base 235.0 RPM
#define HALF_RPM	235.0	//For 12V supply 115.0RPM
#define RPM_COUNT	85		//for 12V supply in the base 42 count  24 v supply 85 count    //in 30 ms
#define MAX_RPM_XY	430		//RPM of robot on xy velocity
#define MIN_RPM_XY	250
#define MAX_RPM_YAW 100		//MAX YAW of the robot



#include "Motor.h"
#include "Encoder.h"
#include "FreeWheelEnc.h"
#include "uart.h"
#include "PID.h"


extern bool pidflag;
 
extern int id,jd;	
 
 extern PID p[4];
 extern Encoder e[4];
 extern FreeWheelEnc ex,ey;	
 extern bool pidflag;
 extern bool count_the_motor;

 extern uint16_t counter_motor;
 
 extern int velocity_robot[4];

 void drive_init();
 void init_timer_ramping();
 void calculate_wheel_velocity();
 void update_wheel_velocity();
 void reset_robot_velocity();
 void reset_motors();


#endif /* DRIVE_H_ */