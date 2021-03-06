//************************************************************************************************************************
// DeepSleep.ino
// Version 1.0 February, 2020
// Author Gerald Guiony
//************************************************************************************************************************

#include <Common.h>
#include <Switches/PushButton.h>


// WARNING : GPIO16 needs to be tied to RST pin to wake from deepSleep (On the NodeMCU, GPIO 16 is represented as D0)

#define PUSH_BUTTON_PIN D1

PUSHBUTTON_CLASS(Test)


//========================================================================================================================
//
//========================================================================================================================
void setup()
{
	// ------------ Global Init

	initSketch();

	// ------------- Setup

	if (I(LoopScheduler).isWakeUpFromDeepSleep())	{
		Logln(F("Wake up from deep sleep mode"));
	}

	I(PushButtonTest).setup (PUSH_BUTTON_PIN);
	I(PushButtonTest).notifyPressedState += []() {
		I(LoopScheduler).enterDeepSleep();
	};

	I(LoopScheduler).setDeepSleepDuration (10000);
	I(LoopScheduler).setup ({&I(PushButtonTest)});
}

//========================================================================================================================
//
//========================================================================================================================
void loop()
{
	I(LoopScheduler).loop ();
}
