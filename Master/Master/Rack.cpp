/*
 * Definations.cpp
 *
 * Created: 5/28/2018 12:11:13 AM
 *  Author: Subash Timilsina
 */ 
#include "Rack.h"



Rack_Motor RackMotor,GenevaMotor;
Rack_Encoder RackEncoder,GenevaEncoder;

unsigned long previous_time;
unsigned long pneumatic_geneva_time;
unsigned long passing_time;

uint8_t Geneva_count;

bool send_time;

PID angle_pid;

volatile unsigned long timer2_millis = 0;
volatile unsigned long timer2_fract = 0;

bool Geneva_Start;
bool Rack_home_position;
bool Buttonx_pressed;
bool donotstop;
bool throw_rack;
bool rack_throw_auto;
bool pneumatic_geneva_start;
bool inside_robot;
bool pass_the_shuttcock;
bool angle_pid_compute;



//initialize global timer

void initialise_timeperiod()
{
	TCCR2B |= (1<<CS22);
	TIMSK2 |= (1<<TOIE2);
	TCNT2 = 0;
}


void rack_init()
{
	
	Geneva_Start = false;
	Buttonx_pressed = false;
	donotstop = false;
	throw_rack = false;
	rack_throw_auto = false;
	pneumatic_geneva_start = false;
	inside_robot = true;
    pass_the_shuttcock = false;
	angle_pid_compute = false;
	
	pneumatic_geneva_time = 0;
	send_time = false;
	
	
	
	RackMotor.Initialise(1);
	GenevaMotor.Initialise(2);
	
	RackEncoder.Initialise(1);
	GenevaEncoder.Initialise(2);
	
	
	INPUT(LTSWITCH_RACK_HOME);
	INPUT(LTSWITCH_RACK_FINAL);
	SET(LTSWITCH_RACK_FINAL);
	SET(LTSWITCH_RACK_HOME);
	
	initialize_pneumatics();
	
	//home position of rack rotate
	while(READ(LTSWITCH_RACK_HOME))
	{
		RackMotor.SetOcrValue(RACK_SPEED_MOTOR);
	}
	
	
	
	previous_time = millis();
	RackMotor.StopMotor();
	RackEncoder.Set_count(0);
	Rack_home_position = true;		// true rack home-position -- initial position and false rack position -- final position
	angle_pid.Set_Pid(47.29,0.139,29.30);
	initialise_timeperiod();
}


void rack_limit_check()
{
	
	if(!READ(LTSWITCH_RACK_HOME) && !Rack_home_position )
	{
		RackMotor.StopMotor();
		Rack_home_position = true;
		RackEncoder.Set_count(0);
		Buttonx_pressed = false;
		throw_rack = true;	
		previous_time = millis();
	}
	if(READ(LTSWITCH_RACK_HOME) && throw_rack && rack_throw_auto)
	{
		RackMotor.SetOcrValue(RACK_SPEED_MOTOR);
	}
	
	if(!READ(LTSWITCH_RACK_FINAL) && Buttonx_pressed && donotstop  )		//reached final position
	{
		RackMotor.StopMotor();
		Buttonx_pressed = false;
		donotstop = false;
		if (rack_throw_auto)
		{
			RACK_LIFT_CLOSE();
			angle_pid.Set_SP(360);
			Geneva_Start = true;
		}
	}
}

void close_all_pneumatics()
{
	RACK_GRIP_OPEN();
	RACK_LIFT_CLOSE();
	SHUTTCOCK_GRIP_CLOSE();
	SHUTTCOCK_PASS_CLOSE();
}

void initialize_pneumatics()
{
	OUTPUT(RACK_GRAB);
	OUTPUT(LIFT_RACK);
	OUTPUT(SC_PASS);
	OUTPUT(SC_GRIPPER);
	close_all_pneumatics();
}

//calculate the time from beggining of robot start 

unsigned long millis()
{
	unsigned long m;
	uint8_t oldSREG = SREG;
	
	// disable interrupts while we read timer2_millis or we might get an
	// inconsistent value 
	cli();
	m = timer2_millis;
	SREG = oldSREG;
	
	return m;
}




//Global timer2 interrupt

ISR(TIMER2_OVF_vect) {
	timer2_millis += 1;
	timer2_fract += 3;
	if (timer2_fract >= 125) {
		timer2_fract -= 125;
		timer2_millis += 1;
	}
}

ISR(INT_VECTR)
{
	if(bit_is_clear(ENCODERR_CHAPORTPIN,ENCODERR_CHBPIN))		//ENCODER_CHAPORTPIN,ENCODER_CHBPIN
	{
		RackEncoder.incCount();
	}
	else
	RackEncoder.dcrCount();
	
}

ISR(INT_VECTG)
{
	if(bit_is_clear(ENCODERG_CHAPORTPIN,ENCODERG_CHBPIN))		//ENCODER_CHAPORTPIN,ENCODER_CHBPIN
	{
		GenevaEncoder.angle++;
		
	}
	else
	{
		GenevaEncoder.angle--;
	}
	
}


ISR(TIMER4_COMPA_vect)
{
	angle_pid_compute = true;
	send_time = true;
}
