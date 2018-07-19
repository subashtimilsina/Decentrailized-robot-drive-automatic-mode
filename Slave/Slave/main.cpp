/*
 * Slave.cpp
 *
 * Created: 6/26/2018 3:49:45 PM
 * Author : Subash Timilsina
 */ 

#include "ZoneNavigate.h"


int main(void)
{
	initUART0();
	initUART2();
	initUART3();
	drive_init();
	init_slave();
	sei();
    while (1) 
    {	
		if(auto_mode)
			operate_slave_auto();
		else
			operate_slave_manual();
			
		calculate_wheel_velocity();
		update_wheel_velocity();
    }
}

