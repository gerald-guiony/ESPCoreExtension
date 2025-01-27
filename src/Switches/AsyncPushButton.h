//************************************************************************************************************************
// AqyncPushButton.h
// Version 2.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

// https://techtutorialsx.com/2016/12/11/esp8266-external-interrupts/

#include <FunctionalInterrupt.h>

#include "Common.h"
#include "Module/AsyncModule.h"


namespace corex {

class AsyncPushButton : public AsyncModule <uint8_t>
{
public:

	AsyncPushButton (uint8_t pin) { setup (pin); }

	/* Interrupts may be attached in all the GPIOs of the ESP8266, except for the GPIO16 [3]											*/
	virtual void setup (uint8_t pin) override
	{

#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
		if (pin == D0 /* 16*/) return;
#endif

		/* When connecting a sensor to a pin configured with INPUT_PULLUP, the other end should be connected to ground.					*/
		/* In the case of a simple switch, this causes the pin to read HIGH when the switch is open, and LOW when the switch is pressed	*/
		/* https://openclassrooms.com/fr/courses/2778161-programmez-vos-premiers-montages-avec-arduino/3285224-le-bouton-poussoir		*/

		/* Attention ne pas utiliser le mode INPUT_PULLUP avec un pin déjà connecté avec une LED et une résistance ! 					*/
		pinMode (pin, ((pin == USER_BTN) || (pin == BLINKLED)) ? INPUT : INPUT_PULLUP);

		/* to trigger when the pin goes from high to low */
		attachInterrupt (digitalPinToInterrupt(pin), [this]() {	notifyPressedState(); }, FALLING);
	}

public:
	Signal <> notifyPressedState;		// WARNµING : You can not use yield or delay or any function that uses them inside the ISR callbacks

};

}