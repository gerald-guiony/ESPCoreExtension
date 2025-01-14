//************************************************************************************************************************
// PushButton.h
// Version 2.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

// https://techtutorialsx.com/2016/12/11/esp8266-external-interrupts/

#include <FunctionalInterrupt.h>

#include "Common.h"


class PushButton : public Module
{
public:

	/* Interrupts may be attached in all the GPIOs of the ESP8266, except for the GPIO16 [3]											*/
	PushButton (uint8_t pin) {

#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
		if (pin == D0 /* 16*/) return;
#endif

		/* When connecting a sensor to a pin configured with INPUT_PULLUP, the other end should be connected to ground.					*/
		/* In the case of a simple switch, this causes the pin to read HIGH when the switch is open, and LOW when the switch is pressed	*/
		/* https://openclassrooms.com/fr/courses/2778161-programmez-vos-premiers-montages-avec-arduino/3285224-le-bouton-poussoir		*/

		/* Attention ne pas utiliser le mode INPUT_PULLUP avec un pin déjà connecté avec une LED et une résistance ! 					*/
		pinMode (pin, ((pin == USER_BTN) || (pin == BLINKLED)) ? INPUT : INPUT_PULLUP);
		/* to trigger when the pin goes from high to low */
		attachInterrupt (digitalPinToInterrupt(pin),
						[this]() {
							if (!_pressedState) {
								_pressedState = true;
								notifyPressedStateAsync();
							}
						},
						FALLING);
	}

	virtual ~PushButton	() {}

	void loop () override {
		if (_pressedState) {
			notifyPressedState ();
			_pressedState = false;
		}
	}

public:
	Signal <> notifyPressedState;
	Signal <> notifyPressedStateAsync;		// WARNµING : You can not use yield or delay or any function that uses them inside the ISR callbacks

protected:
	/* We need to declare a variable as volatile when it can be changed unexpectedly (as in an ISR), so the compiler doesn’t remove		*/
	/* it due to optimizations																											*/
	/* https://barrgroup.com/Embedded-Systems/How-To/C-Volatile-Keyword																	*/
	volatile bool _pressedState		= false;
};