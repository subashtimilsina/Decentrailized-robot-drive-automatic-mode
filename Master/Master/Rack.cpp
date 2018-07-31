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

volatile bool send_time;
volatile bool pid_compute_flag;

volatile bool proximity_on_flag;
volatile bool Goldeneye_on_flag;

PID angle_pid,rack_motor_pid;

volatile unsigned long timer2_millis = 0;
volatile unsigned long timer2_fract = 0;

bool Geneva_Start;
bool Rack_home_position;
bool auto_move_rack;
bool stop_rack_initial;
bool stop_rack_final;
bool throw_rack;
bool rack_throw_auto;
bool rack_pickup;
bool pneumatic_geneva_start;
bool inside_robot;
bool pass_the_shuttcock;




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
	auto_move_rack = false;
	stop_rack_initial = false;
	stop_rack_final = false;
	throw_rack = false;
	rack_throw_auto = false;
	pneumatic_geneva_start = false;
	inside_robot = true;
    pass_the_shuttcock = false;
	pid_compute_flag = false;
					
	
	proximity_on_flag = false;
	Goldeneye_on_flag = false;
	
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
	Rack_home_position = true;  // true rack home-position -- initial position and false rack position -- final position
	
	previous_time = millis();
	
	
	RackEncoder.angle =0;
	
	angle_pid.Set_Pid(47.29,0.139,40.30);	//47.29 0.139  29.30
	rack_motor_pid.Set_Pid(3.08,0,9.89);	//3.08 0 9.89
	
	//Setting the proximity pins
	INPUT(PROXIMITY_PIN);
	SET(PROXIMITY_PIN);
	
	//setting the golden rack drop proximity pin
	INPUT(GOLDENEYE_PIN);
	SET(GOLDENEYE_PIN);
	

	
	initialise_timeperiod();
}


void rack_limit_check()
{
	
	if(!READ(LTSWITCH_RACK_HOME) && stop_rack_initial)	//if reached home position
	{
		RackEncoder.angle = 0;
		rack_motor_pid.Set_SP(0);
		auto_move_rack = false;
		stop_rack_initial = false;
		
		//open rack gripper when reached to home position
		throw_rack = true;
		previous_time = millis();
	}
	
	if(!READ(LTSWITCH_RACK_FINAL) && stop_rack_final)		//if reached final position
	{
		
		rack_motor_pid.Set_SP(0);
		auto_move_rack = false;
		stop_rack_final = false;
		
		if (rack_throw_auto)
		{
			RACK_LIFT_CLOSE();
			rack_throw_auto = false;
		}
	}
	//UART0TransmitData(RackEncoder.angle);
	//UART0TransmitString("\n\r");
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

void enable_proximity()
{
	cli();
	EIMSK |= (1<<PROXIMITY_INT);		//setting INT pin
	EICRB |= (1<<PROXIMITY_ISC1);	    //falling edge
	EIFR |= (1<<PROXIMITY_INTF);	    //clear int flag
	sei();
}

void disable_proximity()
{
	EIFR |= (1<<PROXIMITY_INTF);
	EIMSK &= ~(1<<PROXIMITY_INT);		
}

void enable_golden_eye()
{
	cli();
	EIMSK |= (1<<GOLDENEYE_INT);						//setting INT pin
	EICRA |= ((1<<GOLDENEYE_ISC1)|(1<<GOLDENEYE_ISC0));	//rising  edge
	EIFR |= (1<<GOLDENEYE_INTF);						//clear int flag
	sei();
}

void disable_golden_eye()
{
	EIFR |= (1<<GOLDENEYE_INTF);
	EIMSK &= ~(1<<GOLDENEYE_INT);
}

//calculate the time from begining of robot start 

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

//Rack motor using veitnamese motor
ISR(INT_VECTR)
{
	if(bit_is_clear(ENCODERR_CHAPORTPIN,ENCODERR_CHBPIN))		//ENCODER_CHAPORTPIN,ENCODER_CHBPIN
	{
		RackEncoder.dcrCount();
		RackEncoder.angle--;
	}
	else
	{
		RackEncoder.incCount();
		RackEncoder.angle++;
	}
	
}

//geneva motor cytron

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
	pid_compute_flag = true;
	RackEncoder.Calc_Speed();
	send_time = true;
}
