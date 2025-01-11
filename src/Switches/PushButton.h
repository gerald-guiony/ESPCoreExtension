//************************************************************************************************************************
// PushButton.h
// Version 2.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

// https://techtutorialsx.com/2016/12/11/esp8266-external-interrupts/

#if defined (ESP8266) || defined (ESP32)

#include <FunctionalInterrupt.h>

#include "Common.h"


class PushButton : public Module
{
public:

	/* Interrupts may be attached in all the GPIOs of the ESP8266, except for the GPIO16 [3]											*/
	PushButton (uint8_t pin, std::function <void ()> asyncCallback = [](){} ) :
		_asyncCallback (asyncCallback)	{

		if (pin == 16 /* D0*/) return;

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
								_asyncCallback();
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

protected:
	/* We need to declare a variable as volatile when it can be changed unexpectedly (as in an ISR), so the compiler doesn’t remove		*/
	/* it due to optimizations																											*/
	/* https://barrgroup.com/Embedded-Systems/How-To/C-Volatile-Keyword																	*/
	volatile bool _pressedState		= false;

	/* WARNµING : You can not use yield or delay or any function that uses them inside the callbacks 									*/
	std::function <void ()> _asyncCallback;
};


#endif
