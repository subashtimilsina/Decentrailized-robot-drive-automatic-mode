/*
 * RobotDriver.h
 *
 * Created: 6/25/2018 2:31:46 PM
 *  Author: Subash Timilsina
 */ 


#ifndef ROBOTDRIVER_H_
#define ROBOTDRIVER_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include "Rack.h"
#include "uart.h"
#include "JOYSTICK.h"
#include "Pneumatic.h"

#define START_BYTE_AUTO		254
#define START_BYTE_MANUAL	253

#define AUTO_LED_STRIP		G,0
#define LED_2				G,1
#define LED_3				G,2
#define LED_4				C,0
#define LED_5				C,2
#define MANUAL_LED_STRIP	C,4
#define LED_7				C,6

#define LTSWITCH_ORIENT_1	B,6
#define LTSWITCH_ORIENT_2	B,7
#define LTSWITCH_ORIENT_3	B,5	


extern bool auto_mode;

void init_timer_ramping();
void init_master();
char Get_linetracker_data();
void Send_data_to_Slave();
void operate_master_manual();
void operate_master_auto();
void init_LedStrips();

#endif /* ROBOTDRIVER_H_ */