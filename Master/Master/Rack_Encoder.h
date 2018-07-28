/* 
* Rack_Encoder.h
*
* Created: 5/29/2018 1:55:29 PM
* Author: Subash Timilsina
*/


#ifndef __RACK_ENCODER_H__
#define __RACK_ENCODER_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "headers.h"

#define PPR_CYTRON		180

#define DELAYTIMEMS		20

//Rack encoder

#define  ENCODERR_CHANNELA	D,1
#define	 ENCODERR_CHANNELB	G,5
#define	 ENCODERR_INT		INT1

#define	 ENCODERR_CHAPORTPIN	 PING
#define	 ENCODERR_CHBPIN		 PG5
#define	 INTR_ISC1			ISC11
#define	 INTR_INTF			INTF1

#define  INT_VECTR			INT1_vect

//Geneva encoder
#define  ENCODERG_CHANNELA	D,0
#define	 ENCODERG_CHANNELB	E,3
#define	 ENCODERG_INT		INT0

#define	 ENCODERG_CHAPORTPIN	 PINE
#define	 ENCODERG_CHBPIN		 PE3
#define	 INTG_ISC1			ISC01
#define	 INTG_INTF			INTF0

#define  INT_VECTG			INT0_vect


class Rack_Encoder
{
	private:
	uint8_t no;
	long int speed;
	volatile long int pprcount;
	
	
	public:
	
	volatile long int angle;
	
	Rack_Encoder():speed(0),pprcount(0),angle(0){};
	void Initialise(uint8_t x);
	void Init_Interrupts();
	void Init_Timers();
	inline long int Get_Speed(){return speed;};
	inline void Set_Speed(int val){speed = val;};
	inline void incCount(){pprcount++;};
	inline void dcrCount(){pprcount--;};
	inline int Get_count(){return pprcount;};
	inline void Set_count(int val){pprcount = val;};
	inline void Calc_Speed(){speed = pprcount;pprcount = 0;}; //keep in timer compare match
};

#endif //__RACK_ENCODER_H__
