//************************************************************************************************************************
// DeepSleep.ino
// Version 2.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#include <Common.h>
#include <Switches/PushButton.h>


// WARNING : GPIO16 needs to be tied to RST pin to wake from deepSleep (On the NodeMCU, GPIO 16 is represented as D0)

#define PUSH_BUTTON_PIN D1

PushButton myPushButton (PUSH_BUTTON_PIN);

//========================================================================================================================
//
//========================================================================================================================
void setup()
{
	// ------------ Global Init

	initSketch(true);

	// ------------- Setup

	if (I(ModuleSequencer).isDeviceWakeUpFromDeepSleep())	{
		Logln(F("Wake up from deep sleep mode"));
	}

	myPushButton.notifyPressedState += []() {
		I(ModuleSequencer).enterDeepSleep();
	};

	I(ModuleSequencer).setDeepSleepDuration (10000);  // 10s
	I(ModuleSequencer).setup ({ &myPushButton });
}

//========================================================================================================================
//
//========================================================================================================================
void loop()
{
	I(ModuleSequencer).loop ();
}
