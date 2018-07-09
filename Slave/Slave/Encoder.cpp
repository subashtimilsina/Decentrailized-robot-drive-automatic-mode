/* 
* Encoder.cpp
*
* Created: 10/15/2017 5:57:05 PM
* Author: Subash Timilsina
*/


#include "Encoder.h"

void Encoder::Initialise(uint8_t x)
{	
	no = x;
	if(no==1)
	{
		INPUT(ENCODER1_CHANNELA);
		INPUT(ENCODER1_CHANNELB);
		SET(ENCODER1_CHANNELA);
		SET(ENCODER1_CHANNELB);
	}
	else if(no==2)
	{
		INPUT(ENCODER2_CHANNELA);
		INPUT(ENCODER2_CHANNELB);
		SET(ENCODER2_CHANNELA);
		SET(ENCODER2_CHANNELB);
	}
	else if (no==3)
	{
		INPUT(ENCODER3_CHANNELA);
		INPUT(ENCODER3_CHANNELB);
		SET(ENCODER3_CHANNELA);
		SET(ENCODER3_CHANNELB);
	}
	else if(no==4)
	{
		INPUT(ENCODER4_CHANNELA);
		INPUT(ENCODER4_CHANNELB);
		SET(ENCODER4_CHANNELA);
		SET(ENCODER4_CHANNELB);
	}
	Init_Interrupts();
	Init_Timers();
}

void Encoder::Init_Interrupts()
{
	sei();
	if(no==1)
	{
		EIMSK &= ~(1<<ENCODER1_INT);
		EICRA |= (1<<INT1_ISC1);	//falling edge
		EIMSK |= (1<<ENCODER1_INT);		//setting INT pin
		EIFR |= (1<<INT1_INTF);	    //clear int flag
	}
	else if (no==2)
	{
		EIMSK &= ~(1<<ENCODER2_INT);
		EICRA |= (1<<INT2_ISC1);	//falling edge
		EIMSK |= (1<<ENCODER2_INT);		//setting INT pin
		EIFR |= (1<<INT2_INTF);	    //clear int flag
	}
	else if (no==3)
	{
		EIMSK &= ~(1<<ENCODER3_INT);
		EICRA |= (1<<INT3_ISC1);	//falling edge
		EIMSK |= (1<<ENCODER3_INT);		//setting INT pin
		EIFR |= (1<<INT3_INTF);	    //clear int flag
	}
	else if (no==4)
	{
		EIMSK &= ~(1<<ENCODER4_INT);
		EICRA |= (1<<INT4_ISC1);	//falling edge
		EIMSK |= (1<<ENCODER4_INT);		//setting INT pin
		EIFR |= (1<<INT4_INTF);	    //clear int flag
	}	
}

void Encoder::Init_Timers()
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



