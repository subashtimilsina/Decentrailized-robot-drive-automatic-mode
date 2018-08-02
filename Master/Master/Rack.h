/*
 * Declarations.h
 *
 * Created: 5/29/2018 1:19:29 PM
 *  Author: Subash Timilsina
 */ 


#ifndef DECLARATIONS_H_
#define DECLARATIONS_H_

#include "Pneumatic.h"
#include "Rack_Motor.h"
#include "Rack_Encoder.h"
#include "uart.h"
#include "PID.h"



#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds (a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#define MICROSECONDS_PER_TIMER2_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER2_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER2_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

#define  PROXIMITY_PIN		E,5
#define	 PROXIMITY_INT		INT5
#define	 PROXIMITY_ISC1		ISC51
#define	 PROXIMITY_INTF		INTF5
#define  PROXIMITY_VECT		INT5_vect

#define	  LTSWITCH_RACK_HOME	B,4
#define	  LTSWITCH_RACK_FINAL	H,6

#define   GOLDENEYE_PIN			D,2
#define	  GOLDENEYE_INT			INT2
#define	  GOLDENEYE_ISC1		ISC21
#define   GOLDENEYE_ISC0		ISC20
#define	  GOLDENEYE_INTF		INTF2
#define   GOLDENEYE_VECT		INT2_vect

#define	  RACK_COUNT			 3
#define	  RACK_POSITION_COUNT	 100	//-90
#define	  RACK_SPEED_MOTOR		 50

extern Rack_Motor RackMotor,GenevaMotor;
extern Rack_Encoder RackEncoder,GenevaEncoder;
extern PID angle_pid,rack_motor_pid;

extern bool Geneva_Start;
extern bool Rack_home_position;
extern bool auto_move_rack;
extern bool stop_rack_initial;
extern bool stop_rack_final;
extern bool throw_rack;
extern bool rack_throw_auto;
extern bool pneumatic_geneva_start;
extern bool inside_robot;
extern bool rack_pickup;
extern bool pass_the_shuttcock;

extern volatile bool pid_compute_flag;
extern volatile bool proximity_on_flag;
extern volatile bool Goldeneye_on_flag;


extern unsigned long previous_time;
extern unsigned long pneumatic_geneva_time;
extern unsigned long passing_time;

extern volatile bool send_time;

void rack_init();
void rack_limit_check();
unsigned long millis();
void enable_proximity();
void disable_proximity();
void enable_golden_eye();
void disable_golden_eye();

#endif /* DECLARATIONS_H_ */