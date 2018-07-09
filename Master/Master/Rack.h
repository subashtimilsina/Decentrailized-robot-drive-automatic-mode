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
// the whole number of milliseconds per timer2 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER2_OVERFLOW / 1000)
// the fractional number of milliseconds per timer2 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER2_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)


#define	  LTSWITCH_RACK_HOME	B,4
#define	  LTSWITCH_RACK_FINAL	H,6
#define   GENEVA_LIMIT_SWITCH	D,3

#define	  GENEVA_INT			INT3
#define	  GENEVA_ISC1			ISC31
#define	  GENEVA_INTF			INTF3
#define   GENEVA_VECT			INT3_vect

#define	  RACK_COUNT			-200
#define	  RACK_POSITION_COUNT	-100
#define	  RACK_SPEED_MOTOR		249

extern Rack_Motor RackMotor,GenevaMotor;
extern Rack_Encoder RackEncoder,GenevaEncoder;
extern PID angle_pid;

extern bool Geneva_Start;
extern bool Rack_home_position;
extern bool Buttonx_pressed;
extern bool donotstop;
extern bool throw_rack;
extern bool rack_throw_auto;
extern bool pneumatic_geneva_start;
extern bool inside_robot;
extern bool pass_the_shuttcock;
extern bool angle_pid_compute;


extern unsigned long previous_time;
extern unsigned long pneumatic_geneva_time;
extern unsigned long passing_time;

extern bool send_time;

void rack_init();
void rack_limit_check();
unsigned long millis();

#endif /* DECLARATIONS_H_ */