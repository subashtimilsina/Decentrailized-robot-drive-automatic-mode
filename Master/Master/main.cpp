/*
 * Master.cpp
 *
 * Created: 6/26/2018 3:05:38 PM
 * Author : Subash Timilsina
 */ 

#include "RobotDriver.h"

int main(void)
{
	initUART0();
	initUART3();
	JOYSTICK_INITIALIZE();
	init_master();
	rack_init();
	sei();
    while (1) 
    {
		if(auto_mode)
			operate_master_auto();
		else
			operate_master_manual();
			
		if (send_time)
		{
			Send_data_to_Slave();
			send_time = false;
		}
		
    }
}

