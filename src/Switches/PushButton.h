//************************************************************************************************************************
// PushButton.h
// Version 1.0 October, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

// https://techtutorialsx.com/2016/12/11/esp8266-external-interrupts/


#include "Common.h"

#if defined (ESP8266) || defined (ESP32)


#define ASYNC_PUSHBUTTON_CLASS(className, asyncInstructions)																				\
class PushButton##className	: public Module																									\
{																																			\
public:																																		\
	static PushButton##className & getInstance() {																							\
		static PushButton##className instance; /* Guaranteed to be destroyed, instantiated on first use 								*/	\
		return instance;																													\
	}																																		\
																																			\
	/* Interrupt Service Routines (ISR) handler has to be marked with ICACHE_RAM_ATTR 													*/	\
	static void IRAM_ATTR _ISR_triggerPinFalling ()																							\
	{																																		\
		I(PushButton##className)._pressedState = true;																						\
		asyncInstructions;																													\
	}																																		\
																																			\
	/* Interrupts may be attached in all the GPIOs of the ESP8266, except for the GPIO16 [3]											*/	\
	void setup (uint8_t pin) {																												\
																																			\
		if (pin == 16 /* D0*/) return;																										\
																																			\
		/* When connecting a sensor to a pin configured with INPUT_PULLUP, the other end should be connected to ground.					*/	\
		/* In the case of a simple switch, this causes the pin to read HIGH when the switch is open, and LOW when the switch is pressed	*/	\
		/* https://openclassrooms.com/fr/courses/2778161-programmez-vos-premiers-montages-avec-arduino/3285224-le-bouton-poussoir		*/	\
																																			\
		/* Attention ne pas utiliser le mode INPUT_PULLUP avec un pin déjà connecté avec une LED et une résistance ! 					*/	\
		pinMode (pin, ((pin == USER_BTN) || (pin == BLINKLED)) ? INPUT : INPUT_PULLUP);														\
		/* to trigger when the pin goes from high to low */																					\
		attachInterrupt (digitalPinToInterrupt(pin), PushButton##className::_ISR_triggerPinFalling, FALLING);								\
	}																																		\
																																			\
	void loop () override {																													\
		if (_pressedState) {																												\
			notifyPressedState ();																											\
			_pressedState = false;																											\
		}																																	\
	}																																		\
																																			\
private:																																	\
	PushButton##className						() {}																						\
	virtual ~PushButton##className				() {}																						\
																																			\
public:																																		\
	Signal <> notifyPressedState;																											\
																																			\
protected:																																	\
	/* We need to declare a variable as volatile when it can be changed unexpectedly (as in an ISR), so the compiler doesn’t remove		*/	\
	/* it due to optimizations																											*/	\
	/* https://barrgroup.com/Embedded-Systems/How-To/C-Volatile-Keyword																	*/	\
	volatile bool _pressedState		= false;																								\
};



#define PUSHBUTTON_CLASS(className) 	ASYNC_PUSHBUTTON_CLASS(className, return)


#endif
