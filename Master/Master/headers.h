#ifndef HEADERS_H_
#define HEADERS_H_


#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#define INPUT2(port,pin) DDR ## port &= ~_BV(pin)
#define OUTPUT2(port,pin) DDR ## port |= _BV(pin)
#define CLEAR2(port,pin) PORT ## port &= ~_BV(pin)
#define SET2(port,pin) PORT ## port |= _BV(pin)
#define TOGGLE2(port,pin) PORT ## port ^= _BV(pin)
#define READ2(port,pin) ((PIN ## port & _BV(pin))?1:0)


#define INPUT(x) INPUT2(x)
#define OUTPUT(x) OUTPUT2(x)
#define CLEAR(x) CLEAR2(x)
#define SET(x) SET2(x)
#define TOGGLE(x) TOGGLE2(x)
#define READ(x) READ2(x)



#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>



#endif /* HEADERS_H_ */
