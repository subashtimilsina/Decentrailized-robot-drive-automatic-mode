/* 
* Rack_Encoder.cpp
*
* Created: 5/29/2018 1:55:29 PM
* Author: Subash Timilsina
*/


#include "Rack_Encoder.h"

void Rack_Encoder::Initialise(uint8_t x)
{
	no = x;
	if(no==1)
	{
		INPUT(ENCODERR_CHANNELA);
		INPUT(ENCODERR_CHANNELB);
		SET(ENCODERR_CHANNELA);
		SET(ENCODERR_CHANNELB);
	}
	else if (no==2)
	{
		INPUT(ENCODERG_CHANNELA);
		INPUT(ENCODERG_CHANNELB);
		SET(ENCODERG_CHANNELA);
		SET(ENCODERG_CHANNELB);
	}
	Init_Interrupts();
}

void Rack_Encoder::Init_Interrupts()
{
	sei();
	if (no==1)
	{
		EIMSK &= ~(1<<ENCODERR_INT);
		EICRA |= (1<<INTR_ISC1);	//falling edge
		EIMSK |= (1<<ENCODERR_INT);		//setting INT pin
		EIFR |= (1<<INTR_INTF);	    //clear int flag
	}
	else if (no==2)
	{
		EIMSK &= ~(1<<ENCODERG_INT);
		EICRA |= (1<<INTG_ISC1);	//falling edge
		EIMSK |= (1<<ENCODERG_INT);		//setting INT pin
		EIFR |= (1<<INTG_INTF);	    //clear int flag
	}
	Init_Timers();
}

void Rack_Encoder::Init_Timers()
{
	static bool was_initialized = false;
	if(!was_initialized)
	{
		TCCR4B |= ((1<<WGM42)|(1<<CS40)|(1<<CS41)); //CTC mode and prescaler of 64
		TIMSK4 |= (1<<OCIE4A);
		OCR4A  = 250*DELAYTIMEMS;
		TCNT4 = 0;
		was_initialized = true;
	}
}

