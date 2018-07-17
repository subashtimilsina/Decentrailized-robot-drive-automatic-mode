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


int line_tracker_data;
uint8_t total_on_sensor;
bool val;


volatile int8_t timer_count;													//for joystick ramping
bool rampupflag_start;									//For ramping_the robot using joystick

uint8_t counter_i;

bool auto_mode;

enum Task {static_position,Rack_load,Load1,Load2,Search_automaticrobot,Golden_Rack,Give_shutcock,Give_GoldenRack};
	
enum Location{Loading_zone2 = 1,Loading_zone1,Starting_Zone,Rack_zone,Golden_zone};

Task  slave_work_category;

	


void init_master()
{
	velocity_robot[0] = RESETDATA_JOYSTICK;
	velocity_robot[1] = RESETDATA_JOYSTICK;
	velocity_robot[2] = RESETDATA_JOYSTICK;
	
	slave_work_category = static_position;
	LtState = 0;
	line_tracker_data = 0;
	robot_rpm = 9;		// vary from 0 to 100 contains negative number below 50
	
	auto_mode = false;
	rack_pickup = false;
	//For line tracker pin
	DDRF = 0X00;	//Setting all the digital pin of linetracker to zero
	INPUT(JUNCTION_PIN);
	SET(JUNCTION_PIN);
	
	
	
	init_timer_ramping();
	init_LedStrips();
}

void operate_master_auto()
{
	static unsigned long rack_picktime = 0;
	
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
	
	//if reached rack loading zone
	if(SLAVE_DATA == Rack_zone)
	{
		auto_move_rack = true;
		rack_throw_auto = true;
		SHUTTCOCK_PASS_OPEN();
		RACK_GRIP_CLOSE();
		rack_pickup = true;
		rack_picktime = millis();
		SLAVE_DATA = 0;
	}
	else if(SLAVE_DATA == Golden_zone)
	{
		RACK_GRIP_CLOSE();
		rack_pickup = true;
		rack_picktime = millis();
		SLAVE_DATA = 0;
	}
	else if(SLAVE_DATA == Give_shutcock)
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
	else if(SLAVE_DATA == Give_GoldenRack)
	{
		RACK_GRIP_OPEN();
		SLAVE_DATA = 0;
	}
	
	//delay for rack pickup
	if(rack_pickup && (millis()-rack_picktime) >= 500)
	{
		RACK_LIFT_OPEN();
		
		if(rack_throw_auto)
		{
			rack_motor_pid.Set_SP(RACK_COUNT);
			slave_work_category = Load1;
			rack_pickup = false;
		}	
		else
			slave_work_category = Golden_Rack;	
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
		//front_drive = true;
		
	}
	else if (GAMEBUTTONB == DOWN)
	{
		velocity_robot[0] = RESETDATA_JOYSTICK - robot_rpm;
		velocity_robot[1] = RESETDATA_JOYSTICK;
		velocity_robot[2] = RESETDATA_JOYSTICK;
		//back_drive = true;
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
		else if (LEFTTRIGGER > 20 )
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
		if (RackEncoder.angle <= RACK_POSITION_COUNT && Rack_home_position) // if reached at mid-where somewhere
		{
			Rack_home_position = false;
			donotstop = true;
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
		
		if (rack_throw_auto && Rack_home_position && (millis()-previous_time)>700)
		{
			rack_motor_pid.Set_SP(-RACK_COUNT);
			auto_move_rack = true;
		}
		
		
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
		
		if(throw_rack && (millis()-previous_time) >= 700)
		{
			RACK_GRIP_OPEN();
			throw_rack = false;
			if(rack_throw_auto)
			{
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
	OUTPUT(LED_3);
	OUTPUT(LT3_LED);
	OUTPUT(LED_5);
	OUTPUT(LT2_LED);
	
	CLEAR(AUTO_LED_STRIP);
	CLEAR(MANUAL_LED_STRIP);
	CLEAR(LT1_LED);
	CLEAR(LED_3);
	CLEAR(LT3_LED);
	CLEAR(LED_5);
	CLEAR(LT2_LED);
	
}

/*************************************************************************************LineTracker Section*******************************************************/

void enable_linetracker_interrupt()
{	
	EICRB |= (1<<JUNCTION_ISC1);	//falling edge
	EIMSK |= (1<<JUNCTION_INT);		//setting INT pin
	EIFR |= (1<<JUNCTION_INTF);	    //clear int flag
}

void disable_linetracker_interrupt()
{
	EIMSK &= ~(1<<JUNCTION_INT);
	EIFR |= (1<<JUNCTION_INTF);
}

/**************************************************Get line tracker data and send the value from 10 to 80 or 0 if not found*************************/

char Get_linetracker_data()
{
	line_tracker_data = 0;
	total_on_sensor = 0;
	for(counter_i=0; counter_i < 8; counter_i++)
	{
		val = READ2(F,counter_i);
		line_tracker_data += (val*(counter_i+1)*10);
		total_on_sensor += val;
	}
	total_on_sensor = (total_on_sensor == 0)?1:total_on_sensor;
	line_tracker_data = line_tracker_data/total_on_sensor;
	return line_tracker_data;
}

/*****************************************Function to send data to slave***************************/

void Send_data_to_Slave()
{
	if(auto_mode)
	{
		UART3Transmit(START_BYTE_AUTO);
		UART3Transmit(slave_work_category);
		UART3Transmit(Get_linetracker_data());
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