#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <macros.h>

typedef enum { false, true } bool;

void init_controllers( void ) {

	// TODO: initialise the controllers

}

bool sw2_pressed() {
	
	if (BIT_IS_SET(PINF, 5)) {
		return true;
	} 
	return false;
}

bool sw3_pressed() {

	if (BIT_IS_SET(PINF, 6)) {
		return true;
	} 
	return false;
}

#endif