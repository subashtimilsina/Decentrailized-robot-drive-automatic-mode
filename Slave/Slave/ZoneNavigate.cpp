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
int store_y_count;
float slopeup,slopedown;
int ramp_down_off_adj;
float ramp_up_dist,ramp_down_dist;

Coordinate distance,current_distance,next_distance;

//different task flags
bool moving;
bool searching;
bool Golden_Drop;
bool pressed;
bool calculated;
bool reset_auto;
bool give_shtcock;
bool shift_robot;
bool rotate_robot;
bool go_after_rotate;
bool final_step;
bool stop_flag;
bool prox_enb;



enum Task {static_position,Rack_load,Load1,Load2,Search_automaticrobot,up_rob,down_rob,Golden_Rack,Give_shutcock,Give_GoldenRack,enable_prox,disable_prox};

enum Location{Loading_zone2 = 1,Loading_zone1,Starting_Zone,Rack_zone,Golden_zone};

Location current_location,next_location;
	
	

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
	pressed = false;
	Golden_Drop = false;
	reset_auto = false;
	give_shtcock = false;
	shift_robot = false;
	rotate_robot = false;
	go_after_rotate = false;
	final_step = false;
	stop_flag = false;
	prox_enb = false;
}


void operate_slave_manual()
{
	velocity_robot[0] = (-(DATA1 - 50)/50.0)*MAX_RPM_XY;
	velocity_robot[1] = ((DATA2 - 50)/50.0)*MAX_RPM_XY;
	velocity_robot[2] = (-(DATA3 - 50)/50.0)*MAX_RPM_YAW;
	if(reset_auto)	//reset auto mode data if sowitched from auto to manual
		reset_auto_mode();
		
	//UART2TransmitData(counter_motor);
	//UART2TransmitString("\n\r");
	
}

void operate_slave_auto()
{
	if (!moving && !searching)
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
		else 
		{
			if(!Golden_Drop)
				reset_robot_velocity();
		}
			
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
			DATA1 = 0;
		}
		else if(DATA1 == Golden_Rack)
		{
			Golden_Rack_Place();
			DATA1 = 0;
		}
	}
	
	if(Golden_Drop)
	{
		//move forward to give golden rack
		if(go_after_rotate && (abs(ey.Get_Distance()) >= AUTOROBOT_DIST) )
		{
			reset_robot_velocity();
			rotate_robot = false;
			final_step = false;
			send_data_to_master(Give_GoldenRack);
			Golden_Drop = false;
		}
		
		//move forward after rotating
		if(rotate_robot) 
		{
			if(counter_motor >= ROTATE_COUNT)
			{
				rotate_robot = false;
				go_after_rotate = true;
				velocity_robot[0] = -100;
				velocity_robot[1] = 30;
				velocity_robot[2] = 0;
				ey.Reset_Distance();
				counter_motor = 0;
				count_the_motor = false;
			}
			//else if(counter_motor <= (ROTATE_COUNT >> 1) )
			//{
				//velocity_robot[2] = ((60/1340.0)*counter_motor)-80;
			//}

		}		
		
		//after shifting
		if(shift_robot && ey.Get_Distance() >= Y_DISTANCE)
		{
			reset_robot_velocity();
			shift_robot = false;
			rotate_robot = true;
			count_the_motor = true;
			counter_motor = 0;
			velocity_robot[0] = 0;
			velocity_robot[1] = 0;
			velocity_robot[2] = -100;
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
			
			//if destination is reached interrupt is trigerred by the master
			
			if(stop_flag)
			{
				Brake_the_robot();
			}
			
			//if destination is reached
			if(abs(ex.Get_Distance()) >= distance)
			{
				//stop the robot
				if((current_location == Starting_Zone && next_location == Rack_zone) || (current_location == Loading_zone2 && next_location == Golden_zone))
				{
					velocity_robot[0] = 0;
					velocity_robot[1] = -RAMP_DOWN_OFFSET;
					velocity_robot[2] = 0;
				}
				else
				{
					reset_motors();	
					Brake_the_robot();
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
					if(!prox_enb)
					{
						if((current_location == Starting_Zone && next_location == Rack_zone) || (current_location == Loading_zone2 && next_location == Golden_zone))
							send_data_to_master(enable_prox);
						prox_enb = true;
					}
					velocity_robot[1] = (dir*(slopedown)*ex.Get_Distance())+(dir*ramp_down_off_adj-(dir*ramp_down_off_adj-speed)*distance/ramp_down_dist);
			}
			
	}
	
	//Search the automatic robot
	if(searching)
	{
		if(DATA3 == 0)
		{
			velocity_robot[0] = -searching_rpm;
			velocity_robot[1] = 0;
			velocity_robot[2] = 0;
		}
		else if(DATA3 == 1)
		{
			velocity_robot[0] = 0;
			velocity_robot[1] = 0;
			velocity_robot[2] = -searching_rpm;
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
			velocity_robot[0] = -searching_rpm;
			velocity_robot[1] = 0;
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
	//UART0TransmitData(ramp_down_off_adj);
	//UART0TransmitString("\n\r");
}

void Golden_Rack_Place()
{
	if(current_location == Loading_zone2)
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
	}

}


void move_robot()
{
	dir = CALC_DIR(current_location,next_location);
	ex.Reset_Distance();
	velocity_robot[0] = Y_COMPONENT_INITIAL;
	velocity_robot[2] = 0;
	moving = true;
	
	distance = abs(next_distance - current_distance);
	
	if(distance >= 2600)
	{
		speed = dir*MAX_RPM_XY;
		ramp_down_off_adj = RAMP_DOWN_OFFSET + 30;
	}
	else 
	{
		speed = dir*MIN_RPM_XY;
		ramp_down_off_adj = RAMP_DOWN_OFFSET;
	}
		 
	ramp_up_dist = distance/9.59;
	ramp_down_dist = distance/3.20;	 
	
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
	prox_enb = false;
	calculated = false;
	
	//if reached  different zone different task
	//if(current_location == Rack_zone && next_location == Loading_zone1)
	//{
		//ey.Reset_Distance();
		//_delay_ms(500);
		//searching = true;
		//send_data_to_master(Search_automaticrobot);
		//give_shtcock = true;
	//}
	if(current_location == Starting_Zone && next_location == Rack_zone)
		send_data_to_master(next_location);
	else if(current_location == Golden_zone && next_location == Loading_zone1)
	{
		ey.Reset_Distance();
		shift_robot = true;
		velocity_robot[0] = -130;
		velocity_robot[1] = 0;
		velocity_robot[2] = 0;
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
	
}
