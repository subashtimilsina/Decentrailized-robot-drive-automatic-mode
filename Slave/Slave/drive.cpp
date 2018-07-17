/*
 * drive.cpp
 *
 * Created: 5/29/2018 12:58:09 PM
 *  Author: Subash Timilsina
 */ 

#include "drive.h"

int8_t coupling_matrix[4][3] = {{-1,1,1},{1,1,1},{-1,1,-1},{1,1,-1}};	//just for the direction so kept 1
bool pidflag;		
bool count_the_motor;										 

uint16_t counter_motor;

 int id,jd;					//global loop counter 
 int velocity_motor[4];		//individual motor velocity
 int velocity_robot[4];		//robot velocity
 int ocr_motor[4];			//motor ocr 
 Motor m[4];				//base motors
 Encoder e[4];				// base motors encoders
 FreeWheelEnc ex,ey;		//Free wheel encoders
 PID p[4];					//pid for the base motors
 
 /*					16 bit Timers used 
 ***************	timer 5:(5A 5B 5C for 3 motors) and 3:(3A)motor == base motors
 ***************	timer 1:(1A,1B) == Rack motors
 ***************	timer 4:(4A == pid_computation , 0A == Joystick manual ramping )
*/
 
 /*					base motors and encoders
 **************		motors:
 **************		motor1 -- m[0]	motor2 -- m[1]	motor3 -- m[2]	motor4 -- m[3]
 **************		encoder1 -- e[0] encoder2 -- e[1] encoder3 -- e[2] encoder4 -- e[3]
 **************		velocity_robot[0] -- Vy  velocity_robot[1] -- Vx  velocity_robot[2] -- W
 **************
 */ 
 
 
 /**************************Initialise the drive components************************/
 
 void drive_init()
 {
	 pidflag = false; 
	 for(id=0;id<4;id++)
	 {
		 velocity_motor[id] = 0;
		 velocity_robot[id] = 0;
		 m[id].Initialise(id+1);
		 e[id].Initialise(id+1);
		 p[id].Set_Pid(2.415,0,0.82);	//2.415 , 0 , 0.82
	 }
	 ex.Initialise(1);
	 ey.Initialise(2);
	 counter_motor = 0;
	 count_the_motor = false;
 }
 
 
 
 /***********************Calculation of inverse kinematics****************************************/

  void calculate_wheel_velocity()
  {
	  for(id=0;id<4;id++)
	  {
		  velocity_motor[id] = 0;
		  for(jd=0;jd<3;jd++)
		  {
			  velocity_motor[id] += velocity_robot[jd] * coupling_matrix[id][jd];
		  }
		  
	  }
	  
	  for(id=0;id<4;id++)
	  {
		  ocr_motor[id] = (velocity_motor[id]/MAX_RPM)*RPM_COUNT;
		  p[id].Set_SP(ocr_motor[id]);
	  }
	  
  }
	/*******************************************Updating the calculated velocity in motors********************************************/
  void update_wheel_velocity()
  {  
		if(pidflag)
		{
			for(id = 0; id<4 ; id++)
			{
				m[id].SetOcrValue(p[id].Compute(e[id].Get_Speed()));
			}
			pidflag = false;
		}
  }

void reset_robot_velocity()
{
	for(id = 0; id<4 ; id++)
	{
		 velocity_robot[id] = 0;	//reset the robot velocities
	}
}

void reset_motors()
{
	for(id = 0; id<4 ; id++)
	{
		m[id].StopMotor();
		p[id].reset_output();
	}
}


/*********************************************************************Motors encoders interrupts*****************************************************************/

// motor orientation is alternate so channel a and channel b in two sides are different.

ISR(INT_VECT1)
{
	if(bit_is_clear(ENCODER1_CHAPORTPIN,ENCODER1_CHBPIN))		
	{
		e[0].incCount();
	}
	else
		e[0].dcrCount();
	if(count_the_motor)
		counter_motor++;
}



ISR(INT_VECT2)
{
	if(bit_is_clear(ENCODER2_CHAPORTPIN,ENCODER2_CHBPIN))		
	{
		e[1].incCount();
	}
	else
		e[1].dcrCount();
	
}
ISR(INT_VECT3)
{
	if(bit_is_set(ENCODER3_CHAPORTPIN,ENCODER3_CHBPIN))		
	{
		e[2].incCount();
	}
	else
		e[2].dcrCount();
}

ISR(INT_VECT4)
{
	if(bit_is_set(ENCODER4_CHAPORTPIN,ENCODER4_CHBPIN))		
	{
		e[3].incCount();
	}
	else
		e[3].dcrCount();
}

/*******************************************************************************Free Wheel Encoders Interrupts****************************************************/

ISR(INT_VECTX)
{
	if(bit_is_clear(ENCODERX_CHAPORTPIN,ENCODERX_CHBPIN))
	{
		ex.incCount();
	}
	else
		ex.dcrCount();
}


ISR(INT_VECTY)
{
	if(bit_is_clear(ENCODERY_CHAPORTPIN,ENCODERY_CHBPIN))
	{
		ey.incCount();
	}
	else
		ey.dcrCount();
}

 /*********************************************************PID computation timer*****************************************************/

 ISR(TIMER4_COMPA_vect)
 {
	 e[0].Calc_Speed();
	 e[1].Calc_Speed();
	 e[2].Calc_Speed();
	 e[3].Calc_Speed();
	 pidflag = true;
 }

