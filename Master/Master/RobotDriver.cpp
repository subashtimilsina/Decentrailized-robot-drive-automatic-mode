/*
 * RobotDriver.cpp
 *
 * Created: 7/2/2018 1:48:11 PM
 *  Author: Subash Timilsina
 */ 
#include "RobotDriver.h"




unsigned char velocity_robot[3];
unsigned char robot_rpm;
unsigned char slave_work_category;

int line_tracker_data;
uint8_t total_on_sensor;
bool val;

unsigned long orientation_time;
bool lt_switch_check;
bool lt_switch12_on;
bool lt_switch3_on;
bool limit12_found;

volatile int8_t timer_count;													//for joystick ramping
bool rampupflag_start;									//For ramping_the robot using joystick

uint8_t counter_i;

bool auto_mode;

enum Task {start_position,Rack_load,Load1,Load2,Search_automaticrobot,Golden_Rack,lt12_found,lt3_found};
	
enum Set_Location{staticZone,StZ,RkZ,LdZ1,LdZ2};
	
Set_Location set_current_location;

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
	OUTPUT(LED_2);
	OUTPUT(LED_3);
	OUTPUT(LED_4);
	OUTPUT(LED_5);
	OUTPUT(LED_7);
	
	CLEAR(AUTO_LED_STRIP);
	CLEAR(MANUAL_LED_STRIP);
	CLEAR(LED_2);
	CLEAR(LED_3);
	CLEAR(LED_4);
	CLEAR(LED_5);
	CLEAR(LED_7);
	
}

void init_master()
{
	velocity_robot[0] = RESETDATA_JOYSTICK;
	velocity_robot[1] = RESETDATA_JOYSTICK;
	velocity_robot[2] = RESETDATA_JOYSTICK;
	slave_work_category = 0;
	set_current_location = staticZone;
	line_tracker_data = 0;
	robot_rpm = 9;		// vary from 0 to 100 contains negative number below 50
	auto_mode = false;
	
	limit12_found = false;
	lt_switch12_on = false;
	lt_switch3_on = false;
	lt_switch_check = false;
	orientation_time = 0;
	init_timer_ramping();
	init_LedStrips();
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
		UART3Transmit(set_current_location);
		slave_work_category = 0;
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

void operate_master_manual()
{
	/**********************************************GAMEBUTTONB_SECTION*********************************/
	
	
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
	else if (GAMEBUTTONB == LEFT_BUTTON)
	{
		SHUTTCOCK_GRIP_CLOSE();
		GAMEBUTTONB = 0;
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
		Buttonx_pressed = true;
		if(Rack_home_position)
		{
			RackMotor.SetOcrValue(-RACK_SPEED_MOTOR);
		}
		else
		{
			RackMotor.SetOcrValue(RACK_SPEED_MOTOR);
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
		Buttonx_pressed = true;
		rack_throw_auto = true;
		SHUTTCOCK_PASS_OPEN();
		RACK_LIFT_OPEN();
		RackMotor.SetOcrValue(RACK_SPEED_MOTOR);
		GAMEBUTTONA = 0;
	}
	else if (!pass_the_shuttcock && !rack_throw_auto && !pneumatic_geneva_start && GAMEBUTTONA == RIGHT_BUTTON)	//until the shuttlecock passing completes and until rack auto rack throw completes and until the geneva completes it's rotation
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
		GAMEBUTTONA = 0;
	}

	
	

	/**************************************************Rack Operation******************************************************/
	if (!Buttonx_pressed)
	{
		if (RIGHTTRIGGER > 20 && READ(LTSWITCH_RACK_HOME))
		{
			RackMotor.SetOcrValue(RACK_SPEED_MOTOR);
		}
		else if (LEFTTRIGGER > 20 )
		{
			RackMotor.SetOcrValue(-RACK_SPEED_MOTOR);
		}
		else
		RackMotor.StopMotor();
	}
	

	if (RackEncoder.Get_count() <= RACK_POSITION_COUNT && Rack_home_position) // if reached at mid-where somewhere
	{
		Rack_home_position = false;
		donotstop = true;
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
	
	
	/*******************************************Geneva operation*********************************************/
	
	if (Geneva_Start && GenevaEncoder.angle >= 360 && angle_pid.Get_Error() <= 3)
	{
		if(rack_throw_auto)
		{
			SHUTTCOCK_PASS_CLOSE();
			SHUTTCOCK_GRIP_CLOSE();
			inside_robot = true;
			rack_throw_auto = false;
		}
		GenevaEncoder.angle = 0;
		angle_pid.Set_SP(0);
		angle_pid.reset_iterm();
		angle_pid.reset_output();
		Geneva_Start = false;
	}
	
	if (Geneva_Start && GenevaEncoder.angle >= 100)
	{
		//stop geneva
		pneumatic_geneva_start = false;
	}
	
	if (angle_pid_compute)
	{
		GenevaMotor.SetOcrValue(angle_pid.angle_Compute(GenevaEncoder.angle));
		angle_pid_compute = false;
	}
	
	/*************************************************Delay operation*****************************************/
	
	if (rack_throw_auto && Rack_home_position && (millis()-previous_time)>700)
	{
		RackMotor.SetOcrValue(-RACK_SPEED_MOTOR);
		Buttonx_pressed = true;
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
	}
	

	
	rack_limit_check();

}

void operate_master_auto()
{
	
	if(GAMEBUTTONA == START_BUTTON)
	{
		auto_mode = false;
		GAMEBUTTONA = 0;
	}
	else if(GAMEBUTTONA == BUTTON_A)
	{
		slave_work_category = Rack_load;
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
	else if (GAMEBUTTONA == RIGHT_BUTTON)
	{
		slave_work_category = Search_automaticrobot;
		lt_switch_check = true;
		GAMEBUTTONA = 0;
	}
	
	if(lt_switch_check)
	{
		if(!limit12_found)
		{
			if(!lt_switch12_on && !READ(LTSWITCH_ORIENT_1) && !READ(LTSWITCH_ORIENT_2))
			{
				lt_switch12_on = true;
				orientation_time = millis();
			}
			if (lt_switch12_on && millis()-orientation_time >= 500)
			{
				slave_work_category = lt12_found;
				lt_switch12_on = false;
				limit12_found = true;
			}
		}
		else
		{
			if(!lt_switch3_on && !READ(LTSWITCH_ORIENT_3))
			{
				lt_switch3_on  = true;
				orientation_time = millis();
			}
			if(lt_switch3_on && millis() - orientation_time >= 500)
			{
				slave_work_category = lt3_found;
				lt_switch3_on = false;
				limit12_found = false;
				lt_switch_check = false;
			}
		}
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