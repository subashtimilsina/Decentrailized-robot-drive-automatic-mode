/*
 * Pneumatic.h
 *
 * Created: 5/25/2018 7:35:18 PM
 *  Author: Subash Timilsina
 */ 


#ifndef PNEUMATIC_H_
#define PNEUMATIC_H_

#include "headers.h"


#define RACK_GRAB		A,0
#define LIFT_RACK		A,4
#define SC_GRIPPER		C,7
#define SC_PASS			A,6



#define	RACK_GRIP_CLOSE()			SET(RACK_GRAB)
#define	RACK_GRIP_OPEN()			CLEAR(RACK_GRAB)
#define	RACK_GRIP_TOGGLE()			TOGGLE(RACK_GRAB)

#define	RACK_LIFT_OPEN()			SET(LIFT_RACK)
#define	RACK_LIFT_CLOSE()			CLEAR(LIFT_RACK)
#define	RACK_LIFT_TOGGLE()			TOGGLE(LIFT_RACK)

#define	SHUTTCOCK_PASS_OPEN()		SET(SC_PASS)
#define	SHUTTCOCK_PASS_CLOSE()		CLEAR(SC_PASS)
#define	SHUTTCOCK_PASS_TOGGLE()		TOGGLE(SC_PASS)

#define	SHUTTCOCK_GRIP_OPEN()		SET(SC_GRIPPER)
#define	SHUTTCOCK_GRIP_CLOSE()		CLEAR(SC_GRIPPER)
#define	SHUTTCOCK_GRIP_TOGGLE()		TOGGLE(SC_GRIPPER)

void close_all_pneumatics();
void initialize_pneumatics();



#endif /* PNEUMATIC_H_ */