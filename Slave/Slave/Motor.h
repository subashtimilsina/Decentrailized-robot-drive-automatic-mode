#ifndef MOTOR_H
#define MOTOR_H



	#include "headers.h"

     #define ICR_TOP   249
     #define MAX_VALUE 249
     #define MIN_VALUE -249




	//For motor 1


    #define DD_F1 K,1
    #define DD_B1 K,0

    #define DD_PWM1			L,3
    #define PWM_TCCRA1		TCCR5A
    #define PWM_TCCRB1		TCCR5B
    #define PWM_ICR1			ICR5
    #define PWM_OCR1			OCR5A

    #define PWM_1COM0		COM5A0
    #define PWM_1COM1		COM5A1

    #define PWM_1WGM0		WGM50
    #define PWM_1WGM1		WGM51
    #define PWM_1WGM2		WGM52
    #define PWM_1WGM3		WGM53
    #define PWM_1CS0			CS50
    #define PWM_1CS1			CS51
    #define PWM_1CS2			CS52

    //For motor 2

    #define DD_F2 K,3
    #define DD_B2 K,2

    #define DD_PWM2			L,4
    #define PWM_TCCRA2		TCCR5A
    #define PWM_TCCRB2		TCCR5B
    #define PWM_ICR2			ICR5
    #define PWM_OCR2			OCR5B

    #define PWM_2COM0		COM5B0
    #define PWM_2COM1		COM5B1

    #define PWM_2WGM0		WGM50
    #define PWM_2WGM1		WGM51
    #define PWM_2WGM2		WGM52
    #define PWM_2WGM3		WGM53
    #define PWM_2CS0			CS50
    #define PWM_2CS1			CS51
    #define PWM_2CS2			CS52

//For motor 3

    #define DD_F3 K,4
    #define DD_B3 K,5

    #define DD_PWM3			L,5
    #define PWM_TCCRA3		TCCR5A
    #define PWM_TCCRB3		TCCR5B
    #define PWM_ICR3			ICR5
    #define PWM_OCR3			OCR5C

    #define PWM_3COM0		COM5C0
    #define PWM_3COM1		COM5C1

    #define PWM_3WGM0		WGM50
    #define PWM_3WGM1		WGM51
    #define PWM_3WGM2		WGM52
    #define PWM_3WGM3		WGM53
    #define PWM_3CS0			CS50
    #define PWM_3CS1			CS51
    #define PWM_3CS2			CS52


//For motor 4

    #define DD_F4 K,6
    #define DD_B4 K,7

    #define DD_PWM4			E,3
    #define PWM_TCCRA4		TCCR3A
    #define PWM_TCCRB4		TCCR3B
    #define PWM_ICR4			ICR3
    #define PWM_OCR4			OCR3A

    #define PWM_4COM0		COM3A0
    #define PWM_4COM1		COM3A1

    #define PWM_4WGM0		WGM30
    #define PWM_4WGM1		WGM31
    #define PWM_4WGM2		WGM32
    #define PWM_4WGM3		WGM33
    #define PWM_4CS0			CS30
    #define PWM_4CS1			CS31
    #define PWM_4CS2			CS32



class Motor
{

     private:
		uint8_t num;	
     public:
        void Initialise(uint8_t no);

        void InitPWM();

        void SetForwardDirection();
        void SetReverseDirection();
        void StopMotor();
        void SetOcrValue(int x);
};

#endif // MOTOR_H
