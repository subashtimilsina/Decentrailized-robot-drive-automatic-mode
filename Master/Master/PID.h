/* 
* PID.h
*
* Created: 10/28/2017 11:53:41 AM
* Author: Subash Timilsina
*/


#ifndef __PID_H__
#define __PID_H__

#ifdef abs
#undef abs
#endif

#define abs(x) ((x)>0?(x):-(x))

class PID
{
	private:
		float kp,ki,kd;
		int sp; 
		int error , lastinput , errorsum;
		float cv;
		
	public:
		PID():kp(0),ki(0),kd(0),sp(0),error(0),lastinput(0),errorsum(0){};
		inline void Set_P(float kP){kp = kP;};
		inline void Set_I(float kI){ki = kI;};
		inline void Set_D(float kD){kd = kD;};	
		inline void Set_SP(int sP){sp=sP;};
		inline int Get_SP(){return sp;};
		inline void Inc_P(){kp+=0.1;};
		inline void Inc_I(){ki+=0.001;};
		inline void Inc_D(){kd+=0.1;};
		inline void Dcr_P(){kp-=0.1;};
		inline void Dcr_I(){ki-=0.001;};
		inline void Dcr_D(){kd-=0.1;};
		inline float Get_P(){return kp;};
		inline float Get_I(){return ki;};
		inline float Get_D(){return kd;};
			
		inline float Get_Error(){return abs(error);};
		inline void  reset_error(){error = 0;};
		inline void  reset_iterm(){errorsum = 0;};
		inline void  reset_output(){cv = 0;};
			
		void Set_Pid(float kP,float kI,float kD);
		float Compute(int input);
		float angle_Compute(int input);
		
}; 



#endif //__PID_H__
