/*
 * ZoneNavigate.cpp
 *
 * Created: 7/7/2018 7:54:22 PM
 *  Author: Subash Timilsina
 */ 


#include "ZoneNavigate.h"

int8_t dir;
int speed;
int searching_rpm;
float slopeup,slopedown;
int ramp_down_off_adj;
float ramp_up_dist,ramp_down_dist;

Coordinate distance,current_distance,next_distance;

//different task flags
bool moving;
bool searching;
bool search_fence;
volatile bool Golden_Drop;
bool pressed;
bool calculated;
bool reset_auto;
bool give_shtcock;
bool shift_robot;
bool rotate_robot;
bool go_after_rotate;
volatile bool final_step;
volatile bool stop_flag;
bool line_track_enable;
bool after_move_back;
bool donot_stop_on_line;


enum Task {static_position,Rack_load,Load1,Load2,Search_automaticrobot,up_rob,down_rob,right_rob,left_rob,Golden_Rack,Give_shutcock,enable_prox,enable_gldprox,Stop_Search,Go_after_throw,enable_Jpulse};

enum Location{No_where,Loading_zone2,Loading_zone1,Starting_Zone,Rack_zone,Golden_zone};

Location current_location,next_location,Goto_zone;
	
	

void init_slave()
{	
	auto_mode = false;
	
	INPUT(STOP_THE_DRIVE);
	
	reset_auto_mode();
	enable_robot_stop_interrupt();
}

void reset_auto_mode()
{
	current_location = Starting_Zone;
	next_location = Starting_Zone;
	Goto_zone = No_where;
	
	current_distance = SZONE;
	next_distance = 0;
	
	DATA1 = 50;
	DATA2 = 50;
	DATA3 = 50;
		
	searching_rpm = SEARCH_RPM;
	
	speed = 0;
	
	distance = 0;
	slopeup = 0;
	slopedown = 0;
	ramp_down_off_adj = RAMP_DOWN_OFFSET;
	ramp_up_dist = 0;
	ramp_down_dist = 0;
	calculated = false;

	dir = 1;
	
	ex.Reset_Distance();
	ey.Reset_Distance();
	
	moving = false;
	searching = false;
	search_fence = false;
	pressed = false;
	Golden_Drop = false;
	reset_auto = false;
	give_shtcock = false;
	shift_robot = false;
	rotate_robot = false;
	final_step = false;
	stop_flag = false;
	line_track_enable = false;
	after_move_back = false;
	donot_stop_on_line = false;
}


void operate_slave_manual()
{
	velocity_robot[0] = (-(DATA1 - 50)/50.0)*MAX_RPM_XY;
	velocity_robot[1] = ((DATA2 - 50)/50.0)*MAX_RPM_XY;
	velocity_robot[2] = (-(DATA3 - 50)/50.0)*MAX_RPM_YAW;
	if(reset_auto)	//reset auto mode data if sowitched from auto to manual
		reset_auto_mode();
		
	//UART0TransmitData(ey.Get_Distance());
	//UART0TransmitString("\n\r");
	
}

void operate_slave_auto()
{
	if (!moving && !searching && !search_fence)
	{
		//for up and down purpose adjustment
		if(DATA1 == up_rob)
		{
			velocity_robot[0] = -60;
			velocity_robot[1] = 0;
			velocity_robot[2] = 0;					
		}
		else if(DATA1 == down_rob)
		{
			velocity_robot[0] = 60;
			velocity_robot[1] = 0;
			velocity_robot[2] = 0;  
		} 
		else if(DATA1 == right_rob)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = 60;
			velocity_robot[2] = 0;
		}
		else if(DATA1 == left_rob)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = -60;
			velocity_robot[2] = 0;
		}
		else 
		{
			if(!Golden_Drop)
				reset_robot_velocity();
		}
			
		/******************************Data for auto mode from master************************************/
		if(DATA1 == Rack_load)
		{
			Goto_zone = Rack_zone;
			search_fence = true;
			DATA1 = 0;
		}
		else if(DATA1 == Load1)
		{
			Goto_zone = Loading_zone1;
			search_fence = true;
			DATA1 = 0;
		}
		else if (DATA1 == Load2)
		{
			Goto_zone = Loading_zone2;
			search_fence = true;
			DATA1 = 0;
		}
		else if (DATA1 == Search_automaticrobot)
		{
			searching = true;
			DATA1 = 0;
		}
		else if(DATA1 == Golden_Rack)
		{	
			Golden_Rack_Place();	
			DATA1 = 0;
		}
	}
	
	if(final_step)
	{	
		//after the golden rack is thrown robot will shift back	
		if(DATA1 == Go_after_throw)
		{
			velocity_robot[0] = 60;
			velocity_robot[1] = 30;
			velocity_robot[2] = 0;
			ey.Reset_Distance();
			after_move_back = true;
		}
		
		//stop the robot after certain shift when the rack is thrown
		if(after_move_back && (ey.Get_Distance() <= -330) )
		{
			reset_robot_velocity();
			final_step = false;
			Golden_Drop = false;
			after_move_back = false;
		}
		
		//move forward after rotating
		if(rotate_robot) 
		{
			if(counter_motor >= ROTATE_COUNT)
			{
				rotate_robot = false;
				velocity_robot[0] = -150;
				velocity_robot[1] = 35;
				velocity_robot[2] = 0;
				count_the_motor = false;
				counter_motor = 0;
			}
		}		
		
		//after shifting
		if(shift_robot)
		{
			shift_robot = false;
			rotate_robot = true;
			count_the_motor = true;
			velocity_robot[0] =  0;
			velocity_robot[1] =  0;
			velocity_robot[2] = -150;			
			counter_motor = 0;
			send_data_to_master(enable_gldprox);	
		}
	
	}
	
	if(search_fence)
	{
		if(DATA2 == 2)
		{
			if(Goto_zone == Rack_zone)
			{
				next_location = Rack_zone;
				next_distance = RZONE;
				move_robot();
				search_fence = false;
				Goto_zone = No_where;
			}
			else if(Goto_zone == Loading_zone1)
			{
				next_location = Loading_zone1;
				next_distance = LZONE1;
				move_robot();
				search_fence = false;
				Goto_zone = No_where;
			}
			else if(Goto_zone == Loading_zone2)
			{
				next_location = Loading_zone2;
				next_distance = LZONE2;
				move_robot();
				search_fence = false;
				Goto_zone = No_where;	
			}
		}
		else 
		{
			velocity_robot[0] = SEARCH_RPM;
			velocity_robot[1] = 0;
			velocity_robot[2] = 0;
		}
		
	}
	
	
	/**********************************If moving to different zone then perform ramping************************************/
	if (moving)
	{
		//to calculate slope only one time at the start
			if(!calculated)
			{
				slopeup =  (speed-(dir*RAMP_UP_OFFSET))/ramp_up_dist;
				slopedown = (-((speed-dir*ramp_down_off_adj))/ramp_down_dist);
				calculated = true;
			}
			
			if(!line_track_enable && !donot_stop_on_line && (abs(ex.Get_Distance()) >= (distance-LINETRACK_ENB_DIST)))
			{
				if(next_location == Rack_zone || next_location == Golden_zone)
				{
					send_data_to_master(enable_prox);
				}
				else 
				{
					send_data_to_master(enable_Jpulse);
				}
				line_track_enable = true;
			}
			

			
			//if destination is reached
			if(abs(ex.Get_Distance()) >= distance)
			{
				//stop the robot
				if(donot_stop_on_line)
				{
					Brake_the_robot();
					donot_stop_on_line = false;
				}
				else
				{
					velocity_robot[0] = 0;
					velocity_robot[1] = dir*RAMP_DOWN_OFFSET;
					velocity_robot[2] = 0;
				}
			}
			else if (abs(ex.Get_Distance()) >= ramp_up_dist && (abs(ex.Get_Distance()) <= (distance-ramp_down_dist)))
			{
				//normal
				velocity_robot[1] = speed;
			}
			else if (abs(ex.Get_Distance()) <= ramp_up_dist)
			{
				//ramp up
				velocity_robot[1] = dir*((slopeup*ex.Get_Distance())+RAMP_UP_OFFSET);
			}			
			else if ((abs(ex.Get_Distance()) <= distance) && (abs(ex.Get_Distance()) >= (distance-ramp_down_dist)))
			{
				//ramp down
					velocity_robot[1] = (dir*(slopedown)*ex.Get_Distance())+(dir*ramp_down_off_adj-(dir*ramp_down_off_adj-speed)*distance/ramp_down_dist);
			}
			
			//if destination is reached interrupt is trigerred by the master
			if(stop_flag)
			{
				Brake_the_robot();
			}
			
	}
	
	//Search the automatic robot
	if(searching)
	{
		if(DATA1 == Stop_Search)
		{
			reset_robot_velocity(); 
			searching = false;
			pressed = false;
			searching_rpm = SEARCH_RPM;
			DATA1 = 0;
		}
		
		if(DATA3 == 0)
		{
			velocity_robot[0] = -searching_rpm;
			velocity_robot[1] = 0;
			velocity_robot[2] = 0;				
		}
		else if(DATA3 == 1)
		{
			velocity_robot[0] = -searching_rpm;
			velocity_robot[1] = searching_rpm - 5;
			velocity_robot[2] = 0;
			pressed = true;
		}	
		else if (DATA3 == 2)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = 0;
			velocity_robot[2] = searching_rpm;
			pressed = true;
		}
		else if(DATA3 == 4)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = -searching_rpm;
			velocity_robot[2] = 0;
			pressed = true;
		}
		else if(DATA3 == 3)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = searching_rpm;
			velocity_robot[2] = 0;
			pressed = true;
		}
		else if(DATA3 == 5)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = 0;
			velocity_robot[2] = -searching_rpm;
			pressed = true;		
		}
		else if (DATA3 == 6)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = 0;
			velocity_robot[2] = searching_rpm;
			pressed = true;		
		}
		else if (DATA3 == 7)
		{
			reset_robot_velocity();
			pressed = false;
			searching_rpm = SEARCH_RPM;
			searching = false;
			if(give_shtcock)
			{
				send_data_to_master(Give_shutcock);
				give_shtcock =	false;
			}
		}
		if (pressed)
			searching_rpm = ADJUST_RPM;
	}
		
	reset_auto = true;	
	
	//UART0TransmitData(ex.Get_Distance());
	//UART0Transmit(' ');
	//UART0TransmitData(distance);
	//UART0Transmit(' ');
	//UART0TransmitData(velocity_robot[1]);
	//UART0Transmit(' ');
	//UART0TransmitData(counter_motor);
	//UART0TransmitString("\n\r");
}

void Golden_Rack_Place()
{
	if(current_location == Loading_zone2 || current_location == Starting_Zone)
	{
		Golden_Drop =true;
		next_location = Golden_zone;
		next_distance = GLDZONE;
		move_robot();		
	}
	else if(Golden_Drop && current_location == Golden_zone)
	{
		next_location = Loading_zone1;
		next_distance = LZONE1;
		move_robot();
		donot_stop_on_line = true;
	}

}


void move_robot()
{
	dir = CALC_DIR(current_location,next_location);
	ex.Reset_Distance();
	
	velocity_robot[2] = 0;
	
	moving = true;
	
	distance = abs(next_distance - current_distance);
	
	if(distance >= 3400)
	{
		speed = dir*MAX_RPM_XY;
		ramp_down_off_adj = RAMP_DOWN_OFFSET + 30;
		velocity_robot[0] = Y_COMPONENT_BIG;
	}
	else 
	{
		speed = dir*MIN_RPM_XY;
		ramp_down_off_adj = RAMP_DOWN_OFFSET;
		velocity_robot[0] = Y_COMPONENT_SMALL;
	}
		 
	ramp_up_dist = distance/10.0;			//9.59 factor
	ramp_down_dist = distance/3.20;			//3.20 factor
	
	current_distance = next_distance;
}

void send_data_to_master(uint8_t dat)
{
	UART3Transmit(START_BYTE_MASTER);
	UART3Transmit(dat);
}

void enable_robot_stop_interrupt()
{
	cli();
	PCICR |= (1 << PCIE0);
	PCIFR |= (1 << PCIF0);
	PCMSK0 |= (1 << PCINT4);
	sei();
}

void Brake_the_robot()
{
	distance = 0;
	moving = false;
	calculated = false;
	line_track_enable = false;
	
	/*************************************************************************if reached  different zone different task*****************************/
	
	//After lifting the normal rack search automatic robot and give shuttlecock
	
	//if(current_location == Rack_zone && next_location == Loading_zone1)
	//{
		//searching = true;
		//give_shtcock = true;
	//}
	
	if(current_location == Starting_Zone && next_location == Rack_zone)
		send_data_to_master(next_location);
	else if(current_location == Golden_zone && next_location == Loading_zone1)
	{
		shift_robot = true;
		final_step = true;
	}
	stop_flag = false;
	
	
	if(Golden_Drop)
		send_data_to_master(next_location);
	
	current_location = next_location;	
	ex.Reset_Distance();
}

ISR(PCINT0_vect)
{
	if(moving && !searching && !final_step)
	{
		reset_motors();
		stop_flag = true;
	}
	else if(final_step)
		reset_motors();
}
