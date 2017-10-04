#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <macros.h>
#include "helpers.h"

void init_controllers( void ) {

	// SW2 SW3
    CLEAR_BIT(DDRF, 5);
    CLEAR_BIT(DDRF, 6);

    // LEFT RIGHT UP DOWN CENTER JOYSTICK
    CLEAR_BIT(DDRB, 1);
    CLEAR_BIT(DDRB, 7);
    CLEAR_BIT(DDRB, 0);
    CLEAR_BIT(DDRD, 0);
    CLEAR_BIT(DDRD, 1);

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

bool joyUp_pressed() {
	if (BIT_IS_SET(PIND, 1)) {
		return true;
	}
	return false;
}

bool joyDown_pressed() {
	if (BIT_IS_SET(PINB, 7)) {
		return true;
	}
	return false;
}

bool joyLeft_pressed() {
	if (BIT_IS_SET(PINB, 1)) {
		return true;
	}
	return false;
}

bool joyRight_pressed() {
	if (BIT_IS_SET(PIND, 0)) {
		return true;
	}
	return false;
}

bool joyCentre_pressed() {
	if (BIT_IS_SET(PINB, 0)) {
		return true;
	}
	return false;
}

#endif