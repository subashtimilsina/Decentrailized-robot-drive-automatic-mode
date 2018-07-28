/*
 * RobotDriver.cpp
 *
 * Created: 7/2/2018 1:48:11 PM
 *  Author: Subash Timilsina
 */ 
#include "RobotDriver.h"




unsigned char velocity_robot[3];
unsigned char robot_rpm;
unsigned char LtState;
unsigned char FenceState;


volatile int8_t timer_count;													//for joystick ramping
volatile bool rampupflag_start;									//For ramping_the robot using joystick

uint8_t counter_i;

bool auto_mode;
bool search_auto;
unsigned long search_time;
unsigned long rack_picktime;

enum Task {static_position,Rack_load,Load1,Load2,Search_automaticrobot,up_rob,down_rob,right_rob,left_rob,Golden_Rack,Give_shutcock,enable_prox,enable_gldprox,Stop_Search};
	
enum Location{No_where,Loading_zone2,Loading_zone1,Starting_Zone,Rack_zone,Golden_zone};

Task  slave_work_category;

	


void init_master()
{
	velocity_robot[0] = RESETDATA_JOYSTICK;
	velocity_robot[1] = RESETDATA_JOYSTICK;
	velocity_robot[2] = RESETDATA_JOYSTICK;
	
	slave_work_category = static_position;
	LtState = 0;
	robot_rpm = 9;		// vary from 0 to 100 contains negative number below 50
	search_time = 0;
	rack_picktime = 0;
	
	auto_mode = false;
	search_auto = false;
	rack_pickup = false;
	
	//For line tracker pin
	INPUT(JUNCTION_PIN);
	SET(JUNCTION_PIN);
	
	OUTPUT(STOP_SLAVE);
	
	init_timer_ramping();
	init_LedStrips();
}

void operate_master_auto()
{
	SET(AUTO_LED_STRIP);
	CLEAR(MANUAL_LED_STRIP);
	
	//toggling manual and automatic mode
	if(GAMEBUTTONA == START_BUTTON)
	{
		auto_mode = false;
		GAMEBUTTONA = 0;
	}
	//Automatic zone navigation
	else if(GAMEBUTTONA == BUTTON_A)
	{
		slave_work_category = Rack_load;
		auto_move_rack = true;
		if(Rack_home_position)
			rack_motor_pid.Set_SP(-RACK_COUNT);
		GAMEBUTTONA = 0;
	}
	else if(GAMEBUTTONA == BUTTON_B)
	{
		slave_work_category = Load1;
		GAMEBUTTONA = 0;
	}
	else if (GAMEBUTTONA == BUTTON_X)
	{
		slave_work_category = Load2;
		GAMEBUTTONA = 0;
	}
	else if (GAMEBUTTONA == BUTTON_Y)
	{
		slave_work_category = Golden_Rack;
		GAMEBUTTONA = 0;
	}
	else if (!pass_the_shuttcock && !rack_throw_auto && !pneumatic_geneva_start && GAMEBUTTONA == RIGHT_BUTTON)
	{
		//until the shuttlecock passing completes and until rack auto rack throw completes and until the geneva completes it's rotation
		if(inside_robot)
		{
			pneumatic_geneva_start = true;
			pneumatic_geneva_time = millis();
			SHUTTCOCK_GRIP_TOGGLE();
		}
		else
		{
			SHUTTCOCK_GRIP_CLOSE();
		}
		
		pass_the_shuttcock = true;
		passing_time = millis();
		inside_robot = (1^inside_robot);
		GAMEBUTTONA = 0;
	}

	
	if (GAMEBUTTONB == LEFT_BUTTON)
	{
		slave_work_category = Search_automaticrobot;
		GAMEBUTTONB = 0;
	}	
	else if(GAMEBUTTONB == UP)
	{
		slave_work_category = up_rob;
	}	
	else if(GAMEBUTTONB == DOWN)
	{
		slave_work_category = down_rob;
	}
	else if(GAMEBUTTONB == RIGHT)
	{
		slave_work_category = right_rob;
	}
	else if(GAMEBUTTONB == LEFT)
	{
		slave_work_category = left_rob;
	}
	else if(GAMEBUTTONB == BACK_BUTTON)
	{
		slave_work_category = Stop_Search;
		GAMEBUTTONB = 0;
	}
	
	
	//if reached rack loading zone
	if(SLAVE_DATA == Rack_zone)
	{
		rack_throw_auto = true;
		SHUTTCOCK_PASS_OPEN();
		RACK_GRIP_CLOSE();
		rack_pickup = true;
		rack_picktime = millis();
		disable_proximity();
		SLAVE_DATA = 0;
	}
	else if(SLAVE_DATA == Golden_zone)
	{
		RACK_GRIP_CLOSE();
		disable_proximity();
		rack_pickup = true;
		rack_picktime = millis();
		SLAVE_DATA = 0;
	}
	else if(!pass_the_shuttcock && !rack_throw_auto && !pneumatic_geneva_start && SLAVE_DATA == Give_shutcock)
	{
		if(inside_robot)
		{
			pneumatic_geneva_start = true;
			pneumatic_geneva_time = millis();
			SHUTTCOCK_GRIP_TOGGLE();
		}
		else
		{
			SHUTTCOCK_GRIP_CLOSE();
		}
		
		pass_the_shuttcock = true;
		passing_time = millis();
		inside_robot = (1^inside_robot);
		SLAVE_DATA = 0;
	}
	else if(SLAVE_DATA == enable_prox)
	{
		enable_proximity();		
		SLAVE_DATA = 0;
	}
	else if(SLAVE_DATA == enable_gldprox)
	{
		enable_golden_eye();
		SLAVE_DATA = 0;
	}
	
	
	if(rack_pickup && (millis()-rack_picktime) >= 500)
	{
		RACK_LIFT_OPEN();
		rack_pickup = false;
		
		if(rack_throw_auto)
		{
			auto_move_rack = true;
			if(!Rack_home_position)
				rack_motor_pid.Set_SP(RACK_COUNT);
			slave_work_category = Load1;
		}
		else
		{
			slave_work_category = Golden_Rack;
		}
		
	}
	
	//if not rack auto move stop rack motor
	if(!auto_move_rack)
		rack_motor_pid.Set_SP(0);
		
	operation_of_rack();
	rack_limit_check();
	orientation_check();
}


void operate_master_manual()
{
	/**********************************************GAMEBUTTONB_SECTION*********************************/
	SET(MANUAL_LED_STRIP);
	CLEAR(AUTO_LED_STRIP);
	
	if (GAMEBUTTONB == RIGHT)
	{
		velocity_robot[0] = RESETDATA_JOYSTICK;
		velocity_robot[1] = RESETDATA_JOYSTICK + robot_rpm;
		velocity_robot[2] = RESETDATA_JOYSTICK;
	}
	else if (GAMEBUTTONB == LEFT)
	{
		velocity_robot[0] = RESETDATA_JOYSTICK;
		velocity_robot[1] = RESETDATA_JOYSTICK - robot_rpm;
		velocity_robot[2] = RESETDATA_JOYSTICK;
	}
	else if (GAMEBUTTONB == UP)
	{
		velocity_robot[0] = RESETDATA_JOYSTICK + robot_rpm;
		velocity_robot[1] = RESETDATA_JOYSTICK;
		velocity_robot[2] = RESETDATA_JOYSTICK;
		
	}
	else if (GAMEBUTTONB == DOWN)
	{
		velocity_robot[0] = RESETDATA_JOYSTICK - robot_rpm;
		velocity_robot[1] = RESETDATA_JOYSTICK;
		velocity_robot[2] = RESETDATA_JOYSTICK;
	}
	
	
	/**********************************************************************GAMEBUTTONA_SECTION****************************/
	if(GAMEBUTTONA == START_BUTTON)
	{
		auto_mode = true;
		GAMEBUTTONA = 0;
	}
	else if (GAMEBUTTONA == BUTTON_A)
	{
		RACK_GRIP_TOGGLE();
		GAMEBUTTONA = 0;
	}
	else if (GAMEBUTTONA == BUTTON_B && !Rack_home_position)
	{
		RACK_LIFT_TOGGLE();
		GAMEBUTTONA = 0;
	}
	else if (GAMEBUTTONA == BUTTON_X)
	{
		auto_move_rack = true;
		if(Rack_home_position)
		{
			rack_motor_pid.Set_SP(-RACK_COUNT);
		}
		else
		{
			rack_motor_pid.Set_SP(RACK_COUNT);
		}
		GAMEBUTTONA = 0;
	}
	else if (GAMEBUTTONA == BUTTON_Y)
	{
		angle_pid.Set_SP(360);
		Geneva_Start = true;
		GAMEBUTTONA = 0;
	}
	else if (GAMEBUTTONA == RIGHT_STICK_CLICK && !Rack_home_position)
	{
		auto_move_rack = true;
		rack_throw_auto = true;
		SHUTTCOCK_PASS_OPEN();
		RACK_LIFT_OPEN();
		if(!Rack_home_position)
			rack_motor_pid.Set_SP(RACK_COUNT);
		GAMEBUTTONA = 0;
	}
	else if (!pass_the_shuttcock && !rack_throw_auto && !pneumatic_geneva_start && GAMEBUTTONA == RIGHT_BUTTON)	
	{
		//until the shuttlecock passing completes and until rack auto rack throw completes and until the geneva completes it's rotation
		if(inside_robot)
		{
			pneumatic_geneva_start = true;
			pneumatic_geneva_time = millis();
			SHUTTCOCK_GRIP_TOGGLE();
		}
		else
		{
			SHUTTCOCK_GRIP_CLOSE();
		}
		
		pass_the_shuttcock = true;
		passing_time = millis();
		inside_robot = (1^inside_robot);
		GAMEBUTTONA = 0;
	}

	
	

	/**************************************************Rack Operation******************************************************/
	if (!auto_move_rack)
	{
		if (RIGHTTRIGGER > 20 && READ(LTSWITCH_RACK_HOME))
		{
			rack_motor_pid.Set_SP(RACK_COUNT);
		}
		else if (LEFTTRIGGER > 20 && READ(LTSWITCH_RACK_FINAL))
		{
			rack_motor_pid.Set_SP(-RACK_COUNT);
		}
		else
			rack_motor_pid.Set_SP(0);
	}
	
	/*********************************************************************Move using joystick analog stick********************************************/
	if ((abs(LEFTSTICKY-50) > 5) || (abs(LEFTSTICKX-50) > 5) || abs(RIGHTSTICKX-50)>5)
	{
		rampupflag_start = true;
		velocity_robot[0] = (LEFTSTICKY-50)*timer_count/60.0 + 50;
		velocity_robot[1] = (LEFTSTICKX-50)*timer_count/60.0 + 50;
		velocity_robot[2] = (RIGHTSTICKX-50)*timer_count/60.0 + 50;
	}
	else
	rampupflag_start = false;
	
	operation_of_rack();	
	rack_limit_check();
	orientation_check();

}

void operation_of_rack()
{			
		if (RackEncoder.angle <= RACK_POSITION_COUNT && Rack_home_position) // if reached at mid-where near to the final position
		{
			Rack_home_position = false;
			stop_rack_final = true;
		}	
		else if(RackEncoder.angle >= RACK_POSITION_COUNT && !Rack_home_position) //near to the home position
		{
			Rack_home_position = true;
			stop_rack_initial = true;
		}
		
		/*******************************************Geneva operation*********************************************/
		
		if (Geneva_Start && GenevaEncoder.angle >= 360 && angle_pid.Get_Error() <= 3)
		{
			GenevaEncoder.angle = 0;
			angle_pid.Set_SP(0);
			angle_pid.reset_iterm();
			angle_pid.reset_output();
			Geneva_Start = false;
		}
		
		if(Geneva_Start)
		{
			if( rack_throw_auto && GenevaEncoder.angle >= 200)
			{
				SHUTTCOCK_PASS_CLOSE();
				SHUTTCOCK_GRIP_CLOSE();
				inside_robot = true;
			}
			//stop geneva
			if(GenevaEncoder.angle >= 120)
				pneumatic_geneva_start = false;
		}

		//pid computation
		if (pid_compute_flag)
		{
			GenevaMotor.SetOcrValue(angle_pid.angle_Compute(GenevaEncoder.angle));
			RackMotor.SetOcrValue(rack_motor_pid.Compute(RackEncoder.Get_Speed()));
			pid_compute_flag = false;
		}
		
		/*************************************************Delay operation*****************************************/
		
		
		if(pneumatic_geneva_start && (millis()-pneumatic_geneva_time) > 700)
		{
			angle_pid.Set_SP(360);
			Geneva_Start = true;
		}
		
		if (pass_the_shuttcock && (millis()-passing_time) > 300)
		{
			SHUTTCOCK_PASS_TOGGLE();
			pass_the_shuttcock = false;
		}
		
		if(throw_rack && (millis()-previous_time) >= 300)
		{
			RACK_GRIP_OPEN();
			throw_rack = false;
			if(rack_throw_auto)
			{
				//Rack motor return to it's final position
				rack_motor_pid.Set_SP(-RACK_COUNT);
				auto_move_rack = true;
				
				//Geneva start after rack placement
				angle_pid.Set_SP(360);
				Geneva_Start = true;
			}
		}
}

//Timer for ramping interrupt in 10ms
//Global timer
void init_timer_ramping()
{
	rampupflag_start = false;
	timer_count = 0;
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS00)|(1<<CS02);	//1024 prescaler CTC mode
	TIMSK0 |= (1<<OCIE0A);
	TCNT0 = 0;
	OCR0A = 155;
}

/****************************************Initialise the master ***************************************/
void init_LedStrips()
{
	OUTPUT(AUTO_LED_STRIP);
	OUTPUT(MANUAL_LED_STRIP);
	OUTPUT(LT1_LED);
	OUTPUT(LED_1);
	OUTPUT(LT3_LED);
	OUTPUT(GOLDEN_LED);
	OUTPUT(LT2_LED);
	
	CLEAR(AUTO_LED_STRIP);
	CLEAR(MANUAL_LED_STRIP);
	CLEAR(LT1_LED);
	CLEAR(LED_1);
	CLEAR(LT3_LED);
	CLEAR(GOLDEN_LED);
	CLEAR(LT2_LED);
	
}

/*************************************************************************************LineTracker Section*******************************************************/

void enable_linetracker_interrupt()
{	
	cli();
	EICRB |= (1<<JUNCTION_ISC1);	//falling edge
	EIMSK |= (1<<JUNCTION_INT);		//setting INT pin
	EIFR |= (1<<JUNCTION_INTF);	    //clear int flag
	sei();
}

void disable_linetracker_interrupt()
{
	EIMSK &= ~(1<<JUNCTION_INT);
	EIFR |= (1<<JUNCTION_INTF);
}

/*****************************************Function to send data to slave***************************/

void Send_data_to_Slave()
{
	if(auto_mode)
	{
		UART3Transmit(START_BYTE_AUTO);
		UART3Transmit(slave_work_category);
		UART3Transmit(FenceState);
		UART3Transmit(LtState);
		slave_work_category = static_position;
	}
	else
	{
		UART3Transmit(START_BYTE_MANUAL);
		UART3Transmit(velocity_robot[0]);
		UART3Transmit(velocity_robot[1]);
		UART3Transmit(velocity_robot[2]);
		velocity_robot[0] = RESETDATA_JOYSTICK;
		velocity_robot[1] = RESETDATA_JOYSTICK;
		velocity_robot[2] = RESETDATA_JOYSTICK;
	}

	
}

void orientation_check()
{
		//limit_switch 1
		if(READ(LTSWITCH_ORIENT_1))
		{
			LtState &= ~(1<<0);
			CLEAR(LT1_LED);
		}
		else
		{
			LtState |= (1<<0);
			SET(LT1_LED);
		}
		
		//limit_Switch 2
		if(READ(LTSWITCH_ORIENT_2))
		{
			LtState &= ~(1<<1);
			CLEAR(LT2_LED);
		}
		else
		{
			LtState |= (1<<1);
			SET(LT2_LED);
		}
		
		//limit_Switch 3
		if (READ(LTSWITCH_ORIENT_3))
		{
			LtState &= ~(1<<2);
			CLEAR(LT3_LED);
		}
		else
		{
			LtState |= (1<<2);
			SET(LT3_LED);
		}
		
		//fence limit switches
		if(!READ(LTSWITCH_FENCE_1) && !READ(LTSWITCH_FENCE_2))
			FenceState = 2;
		else 
			FenceState = 0;
			
}



//Global timer interrupt

/***********************************Ramping from the joystick analog button*************************************/

ISR(TIMER0_COMPA_vect)
{
	if (rampupflag_start)
	timer_count++;
	else
	timer_count = 0;
	
	if (timer_count > 60)
	timer_count = 60;
	

}

/**********************************************************PROXIMITY_INTERRUPT***********************************************************************/
ISR(PROXIMITY_VECT)
{
	//pcint for slave on to stop the robot
	TOGGLE(STOP_SLAVE);
}

ISR(GOLDENEYE_VECT)
{
	RACK_GRIP_OPEN();
	TOGGLE(STOP_SLAVE);
	disable_golden_eye();
}

/*************************************************************Line-tracker junction interrupt****************************************************************/
