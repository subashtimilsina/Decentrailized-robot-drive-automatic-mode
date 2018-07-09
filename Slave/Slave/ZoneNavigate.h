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
******************DATA1 -- Slave_Work_Category, DATA2 -- Linetracker_Data, DATA3 --	Set-Current zone	*******************/

#define DATA1	rcvdata[0]
#define DATA2	rcvdata[1]
#define DATA3	rcvdata[2]

//calculating distance count using formula : (destination/WHEEL_CIRCUMFERENCE)*PPR;
/****************************RZ -- 3000mm, LZ1 -- 11000mm, LZ2 -- 11500mm   Wheel_circumference = pi*d = 58*3.141592 = 182.21237*******************/
#define SZONE	3292
#define RZONE	0
#define LZONE1	12073
#define LZONE2	12622

#define Y_COMPONENT	 30	
#define SEARCH_RPM	 40
#define RAMP_OFFSET	 50


#define CALC_DIR(x,y)	(((x-y) < 0)? -1:1)

typedef long int Coordinate;
extern bool auto_mode;

void init_slave();
void operate_slave_manual();
void operate_slave_auto();
void move_robot();
void reset_auto_mode();
void Golden_Rack_Place();

#endif /* ZONENAVIGATE_H_ */