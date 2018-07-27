/*
 * ZoneNavigate.h
 *
 * Created: 6/30/2018 9:51:11 AM
 *  Author: Subash Timilsina
 */ 


#ifndef ZONENAVIGATE_H_
#define ZONENAVIGATE_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include "drive.h"
#include <util/delay.h>
#include "headers.h"

/***********************************************Manual mode**************************************************************
******************DATA1 -- velocity_robot[0] , DATA2 -- velocity_robot[1] , DATA3 -- velocity_robot[2] ******************
************************************************Automatic mode***********************************************************
******************DATA1 -- Slave_Work_Category, DATA2 -- Linetracker_Data, DATA3 --	Limit-switch state	*******************/


/**************************************************** sign and the direction of robot********************************************/
// velocity_robot[0]  +ve back  and -ve front
// velocity_robot[1]  +ve right and -ve left
// velocity_robot[2]  +ve anti-clockwise and -ve clockwise
/********************************************************************************************************************************/

#define DATA1	rcvdata[0]
#define DATA2	rcvdata[1]
#define DATA3	rcvdata[2]

//calculating distance count using formula : (destination/WHEEL_CIRCUMFERENCE)*PPR;
/***********RZ -- 2265mm, LZ1 -- 6974mm, LZ2 --9196 mm  Wheel_circumference d=57.9323 then pi*d=182 ---- d =58 then pi*d = 58*3.141592 = 182.21237*** 11 count = 10 mm********/

#define SZONE		2490	//2520
#define RZONE		0
#define GLDZONE		-770
#define LZONE1		(6865 + SZONE)		//6975
#define LZONE2		(2220 + LZONE1)		//2220

#define Y_DISTANCE		220
#define ROTATE_COUNT	800			//1340
#define AUTOROBOT_DIST	1940	

#define Y_COMPONENT_SMALL		 15
#define Y_COMPONENT_BIG			 30	 


#define SEARCH_RPM				 60
#define ADJUST_RPM				 20

#define RAMP_UP_OFFSET			 80
#define RAMP_DOWN_OFFSET		 25		

#define START_BYTE_MASTER		252

#define STOP_THE_DRIVE			B,4

#define CALC_DIR(x,y)	(((x-y) < 0)? -1:1)

typedef long int Coordinate;
extern bool auto_mode;


void init_slave();
void operate_slave_manual();
void operate_slave_auto();
void move_robot();
void reset_auto_mode();
void send_data_to_master(uint8_t dat);
void Golden_Rack_Place();
void enable_robot_stop_interrupt();
void Brake_the_robot();

#endif /* ZONENAVIGATE_H_ */