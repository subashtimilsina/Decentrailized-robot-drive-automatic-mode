/*
 * JOYSTICK.h
 *
 * Created: 1/13/2018 2:30:27 AM
 *  Author: Subash Timilsina
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#define JOYSTICK_INITIALIZE()		initUART2()

//GAMEBUTTONA
#define BUTTON_A			1
#define BUTTON_B			2
#define BUTTON_X			4
#define BUTTON_Y			8
#define RIGHT_STICK_CLICK	16
#define RIGHT_BUTTON		32
#define START_BUTTON		64

//Gamebutton B
#define UP					1
#define RIGHT				2
#define DOWN				4
#define LEFT				8
#define LEFT_STICK_CLICK	16
#define LEFT_BUTTON			32
#define BACK_BUTTON			64

#define GAMEBUTTONA			rcvdata[0]
#define GAMEBUTTONB			rcvdata[1]
#define	RIGHTTRIGGER		rcvdata[2]
#define LEFTTRIGGER			rcvdata[3]
#define LEFTSTICKX			rcvdata[4]
#define LEFTSTICKY			rcvdata[5]
#define RIGHTSTICKX			rcvdata[6]
#define RIGHTSTICKY			rcvdata[7]


#define RESETDATA_JOYSTICK	50


#endif /* JOYSTICK_H_ */