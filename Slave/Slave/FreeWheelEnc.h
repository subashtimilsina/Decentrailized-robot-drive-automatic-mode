/* 
* FreeWheelEnc.h
*
* Created: 7/5/2018 1:01:18 PM
* Author: Subash Timilsina
*/


#ifndef __FREEWHEELENC_H__
#define __FREEWHEELENC_H__


#include <avr/io.h>
#include <avr/interrupt.h>
#include "headers.h"


#define	 FREE_ENC_PPR				200.0

//encoder x

#define  ENCODERX_CHANNELA	E,5
#define	 ENCODERX_CHANNELB	C,7		
#define	 ENCODERX_INT		INT5		

#define	 ENCODERX_CHAPORTPIN	 PINC
#define	 ENCODERX_CHBPIN		 PC7
#define	 INTX_ISC1			ISC51
#define	 INTX_INTF			INTF5

#define  INT_VECTX			INT5_vect


//encoder y

#define  ENCODERY_CHANNELA	E,4
#define	 ENCODERY_CHANNELB	C,5
#define	 ENCODERY_INT		INT4

#define	 ENCODERY_CHAPORTPIN	 PINC
#define	 ENCODERY_CHBPIN		 PC5
#define	 INTY_ISC1			ISC41
#define	 INTY_INTF			INTF4

#define  INT_VECTY			INT4_vect



class FreeWheelEnc
{
	
		private:
		uint8_t no;
		long int encdistance;
		public:
		FreeWheelEnc():encdistance(0){};
		void Initialise(uint8_t x);
		void Init_Interrupts();
		inline void incCount(){encdistance++;};
		inline void dcrCount(){encdistance--;};
		inline long int Get_Distance(){return encdistance;};
		inline void Reset_Distance(){encdistance = 0;}

}; //FreeWheelEnc

#endif //__FREEWHEELENC_H__
