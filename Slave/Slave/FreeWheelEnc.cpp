/* 
* FreeWheelEnc.cpp
*
* Created: 7/5/2018 1:01:18 PM
* Author: Subash Timilsina
*/


#include "FreeWheelEnc.h"

void FreeWheelEnc::Initialise(uint8_t x)
{
	no = x;
	if(no==1)
	{
		INPUT(ENCODERX_CHANNELA);
		INPUT(ENCODERX_CHANNELB);
		SET(ENCODERX_CHANNELA);
		SET(ENCODERX_CHANNELB);
	}
	else if(no==2)
	{
		INPUT(ENCODERY_CHANNELA);
		INPUT(ENCODERY_CHANNELB);
		SET(ENCODERY_CHANNELA);
		SET(ENCODERY_CHANNELB);
	}
	Init_Interrupts();
}

void FreeWheelEnc::Init_Interrupts()
{
	if(no==1)
	{
		EIMSK &= ~(1<<ENCODERX_INT);
		EICRB |= (1<<INTX_ISC1);	//falling edge
		EIMSK |= (1<<ENCODERX_INT);		//setting INT pin
		EIFR  |= (1<<INTX_INTF);	    //clear int flag
	}
	else if (no==2)
	{
		EIMSK &= ~(1<<ENCODERY_INT);
		EICRB |= (1<<INTY_ISC1);	//falling edge
		EIMSK |= (1<<ENCODERY_INT);		//setting INT pin
		EIFR |= (1<<INTY_INTF);	    //clear int flag
	}
	sei();
}
