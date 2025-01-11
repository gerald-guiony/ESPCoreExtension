//************************************************************************************************************************
// PushButton.ino
// Version 2.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#include <Common.h>
#include <Switches/PushButton.h>

// Ouput in the Arduino IDE serial monitor :
//
// [t:76ms ESP8822001] ******* Chip is (re)booting *******
// [t:5319ms ESP8822001] Interrupt (ISR) : Button was pressed
// [t:6158ms ESP8822001] Thread loop : Button was pressed
//
// => the message appears instantly with the ISR method when the button is pressed

#define PUSH_BUTTON_PIN		D1

PushButton myPushButton (PUSH_BUTTON_PIN, [](){ Logln (F("Interrupt (ISR) : Button was pressed"));} );

//========================================================================================================================
//
//========================================================================================================================
void setup()
{
	// ------------ Global Init

	initSketch (true);

	// ------------- Setup

	myPushButton.notifyPressedState += []() {
		Logln (F("Thread loop : Button was pressed"));
	};
	
	I(ModuleSequencer).setup ({ &myPushButton });
}

//========================================================================================================================
//
//========================================================================================================================
void loop()
{
	I(ModuleSequencer).loop ();
}
