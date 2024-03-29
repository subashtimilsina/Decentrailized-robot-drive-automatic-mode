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
#include "headers.h"

/***********************************************Manual mode**************************************************************
******************DATA1 -- velocity_robot[0] , DATA2 -- velocity_robot[1] , DATA3 -- velocity_robot[2] ******************
************************************************Automatic mode***********************************************************
******************DATA1 -- Slave_Work_Category, DATA2 -- Linetracker_Data, DATA3 --	Limit-switch state	*******************/

#define DATA1	rcvdata[0]
#define DATA2	rcvdata[1]
#define DATA3	rcvdata[2]

//calculating distance count using formula : (destination/WHEEL_CIRCUMFERENCE)*PPR;
/***********RZ -- 2265mm, LZ1 -- 6974mm, LZ2 --9196 mm  Wheel_circumference d=57.9323 then pi*d=182 ---- d =58 then pi*d = 58*3.141592 = 182.21237*** 11 count = 10 mm********/

#define SZONE		2490	//2520
#define RZONE		0
#define GLDZONE		-770
#define LZONE1		(6975 + SZONE)		//9590
#define LZONE2		(2220 + LZONE1)

#define Y_DISTANCE		1200
#define ROTATE_COUNT	1340
#define AUTOROBOT_DIST	1400	

#define Y_COMPONENT_INITIAL		 15	 


#define SEARCH_RPM				 60
#define ADJUST_RPM				 20

#define RAMP_UP_OFFSET			 80
#define RAMP_DOWN_OFFSET		 25		//20 for rack loading zone

#define RAMP_UP_DIST			(distance/9.59)
#define RAMP_DOWN_DIST			(distance/3.20)

#define START_BYTE_MASTER		252

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

#endif /* ZONENAVIGATE_H_ */