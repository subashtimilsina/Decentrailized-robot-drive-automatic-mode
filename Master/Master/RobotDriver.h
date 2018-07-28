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

#define SLAVE_DATA			rcv_value_slave

#define START_BYTE_AUTO		254
#define START_BYTE_MANUAL	253

#define AUTO_LED_STRIP		G,0
#define LT2_LED				G,1
#define LED_1				G,2
#define LT3_LED				C,0
#define GOLDEN_LED			C,2
#define MANUAL_LED_STRIP	C,6
#define LT1_LED				C,4

#define LTSWITCH_ORIENT_1	B,6
#define LTSWITCH_ORIENT_2	B,7
#define LTSWITCH_ORIENT_3	B,5	

#define LTSWITCH_FENCE_1	D,3
#define LTSWITCH_FENCE_2	H,5

#define STOP_SLAVE			L,2

#define  JUNCTION_PIN		E,4
#define	 JUNCTION_INT		INT4
#define	 JUNCTION_ISC1		ISC41
#define	 JUNCTION_INTF		INTF4
#define  JUNCTION_VECT		INT4_vect


extern bool auto_mode;

void init_timer_ramping();
void init_master();
void enable_linetracker_interrupt();
void disable_linetracker_interrupt();
void init_LedStrips();
char Get_linetracker_data();
void Send_data_to_Slave();
void orientation_check();
void operation_of_rack();
void operate_master_manual();
void operate_master_auto();

#endif /* ROBOTDRIVER_H_ */