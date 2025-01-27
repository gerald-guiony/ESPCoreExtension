//************************************************************************************************************************
// CriticalSection.h
// Version 1.0 April, 2019
// Author Gerald Guiony
//************************************************************************************************************************


#pragma once

#include <Arduino.h>


namespace corex {
/*
* noInterrupts() is the safe and only way to synchronize with the interrupt method
* \Author GGU
* \Date 22/04/2019
*/
class CriticalSection {
public :
	CriticalSection ()	{
		noInterrupts();						// Some functions will not work while interrupts are disabled, and incoming communication may be ignored.
											// Interrupts can slightly disrupt the timing of code, however, and may be disabled for particularly critical
											// sections of code.
	}

	~CriticalSection () {
		interrupts();						// Re-enables interrupts (after they've been disabled by noInterrupts()). Interrupts allow certain important
											// tasks to happen in the background and are enabled by default.
	}
};

}