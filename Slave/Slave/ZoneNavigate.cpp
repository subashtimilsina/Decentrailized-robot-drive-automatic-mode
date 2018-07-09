/*
 * ZoneNavigate.cpp
 *
 * Created: 7/7/2018 7:54:22 PM
 *  Author: Subash Timilsina
 */ 


#include "ZoneNavigate.h"


char master_work_category;
int8_t dir;
int speed;
float slope;

Coordinate distance,current_distance,next_distance;

bool moving;
bool searching;
bool started;
bool Golden_Drop;
bool calculated;
bool reset_auto;


enum Task {start_position,Rack_load,Load1,Load2,Search_automaticrobot,Golden_Rack,lt12_found,lt3_found};

enum Location{Loading_zone2 = 1,Loading_zone1,Starting_Zone,Rack_zone};
	
enum Set_Location{staticZone,StZ,RkZ,LdZ1,LdZ2};

Location current_location,next_location;
	
	

void init_slave()
{
	DATA1 = 50;
	DATA2 = 50;
	DATA3 = 50;
	
	auto_mode = false;
	master_work_category = 0;
	
	speed = 0;
	current_location = Starting_Zone;
	next_location = Starting_Zone;
	
	distance = 0;
	slope = 0;
	calculated = false;
	
	current_distance = SZONE;
	next_distance = SZONE;
	dir = 1;
	
	ex.Reset_Distance();
	
	moving = false;
	searching = false;
	started = false;
	Golden_Drop = false;
	reset_auto = false;
}

void operate_slave_manual()
{
	velocity_robot[0] = (-(DATA1 - 50)/50.0)*MAX_RPM_XY;
	velocity_robot[1] = ((DATA2 - 50)/50.0)*MAX_RPM_XY;
	velocity_robot[2] = (-(DATA3 - 50)/50.0)*MAX_RPM_YAW;
	if(reset_auto)	//reset auto mode data if switched from auto to manual
		reset_auto_mode();
}

void operate_slave_auto()
{
	if (!moving && !searching)
	{
		/******************************Data for auto mode from master************************************/
		if(DATA1 == Rack_load)
		{
			next_location = Rack_zone;
			next_distance = RZONE;
			move_robot();
			DATA1 = 0;
		}
		else if(DATA1 == Load1)
		{
			next_location = Loading_zone1;
			next_distance = LZONE1;
			move_robot();
			DATA1 = 0;
		}
		else if (DATA1 == Load2)
		{
			next_location = Loading_zone2;
			next_distance = LZONE2;
			move_robot();
			DATA1 = 0;
		}
		else if (DATA1 == Search_automaticrobot)
		{
			searching = true;
			started = true;
			DATA1 = 0;
		}
	}
	
	/**********************************If moving to different zone then perform ramping************************************/
	if (moving)
	{
		//to calculate slope only one time at the start
			if(!calculated)
			{
				slope =  ((8.0*(speed-dir*RAMP_OFFSET))/distance);
				calculated = true;
			}
			
			//if destination is reached
			
			if(abs(ex.Get_Distance()) >= distance)
			{
				//stop the robot
				reset_robot_velocity();
				ex.Reset_Distance();
				moving = false;
				calculated = false;
			}
			else if (abs(ex.Get_Distance()) >= (distance>>3) && (abs(ex.Get_Distance()) <= 7*(distance>>3)))
			{
				//normal
				velocity_robot[1] = speed;
				
			}
			else if (abs(ex.Get_Distance()) <= (distance>>3))
			{
				//ramp up
				velocity_robot[1] = dir*((slope*ex.Get_Distance())+RAMP_OFFSET);
			}
			
			else if ((abs(ex.Get_Distance()) <= distance) && abs((ex.Get_Distance())) >= 7*(distance>>3))
			{
				//ramp down
					velocity_robot[1] = (dir*(-slope)*ex.Get_Distance())+(8*speed)-(7*dir*RAMP_OFFSET);
			}	
			
	}
	
	if(searching)
	{
		if (started)
		{
			velocity_robot[0] = -SEARCH_RPM;
			velocity_robot[1] = 0;
			velocity_robot[2] = 0;
		}
		if(DATA1 == lt12_found)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = SEARCH_RPM;
			velocity_robot[2] = 0;	
			DATA1 = 0;	
			started = false;
		}
		else if(DATA1 == lt3_found)
		{
			reset_robot_velocity();
			searching = false;
			DATA1 = 0;	
		}
		

	}
	
	reset_auto = true;	
}




void move_robot()
{
	dir = CALC_DIR(current_location,next_location);
	ex.Reset_Distance();
	velocity_robot[0] = Y_COMPONENT;
	speed = dir*HALF_RPM;
	velocity_robot[2] = 0;
	moving = true;
	current_location = next_location;
	distance = abs(next_distance - current_distance); 
	current_distance = next_distance;
}

void reset_auto_mode()
{
	speed = 0;
	current_location = Starting_Zone;
	next_location = Starting_Zone;
	
	distance = 0;
	slope = 0;
	calculated = false;
	
	current_distance = SZONE;
	next_distance = 0;
	dir = 1;
	
	ex.Reset_Distance();
	
	moving = false;
	searching = false;
	started = false;
	Golden_Drop = false;
	reset_auto = false;
}


void Golden_Rack_Place()
{
		
}