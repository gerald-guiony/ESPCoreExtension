//************************************************************************************************************************
// Jumper.h
// Version 1.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include "Common.h"


namespace corex {

class Jumper
{
private:
	uint8_t _pin;

public:

	Jumper (uint8_t pin) : _pin (pin)
	{

#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
		// Pas de PWM ..
		if (pin == D0 /* 16*/) return;
#endif

		/* When connecting a sensor to a pin configured with INPUT_PULLUP, the other end should be connected to ground.					*/
		/* In the case of a simple switch, this causes the pin to read HIGH when the switch is open, and LOW when the switch is pressed	*/
		/* https://openclassrooms.com/fr/courses/2778161-programmez-vos-premiers-montages-avec-arduino/3285224-le-bouton-poussoir		*/

		/* Attention ne pas utiliser le mode INPUT_PULLUP avec un pin déjà connecté avec une LED et une résistance ! 					*/
		pinMode (pin, ((pin == USER_BTN) || (pin == BLINKLED)) ? INPUT : INPUT_PULLUP);
	}

	bool isConnectedToGround () const 	{ return (digitalRead (_pin) == LOW); }

};

}