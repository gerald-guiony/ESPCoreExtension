//************************************************************************************************************************
// ModuleSequencer.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

// http://bbs.espressif.com/viewtopic.php?f=7&t=171&p=617
// http://jeelabs.org/book/1526f/
// http://blog.mister-bidouilles.fr/surveillez-la-consommation-de-vos-montages/


#pragma once

#include <list>

#include "Tools/Singleton.h"
#include "Tools/Signal.h"

#include "Module.h"


#define SHORT_DELAY_BETWEEN_EXEC_S					1
#define LONG_DELAY_BETWEEN_EXEC_S					10

#define DELAY_BETWEEN_WAKEUP_S						60

#ifdef ESP8266
#	define DEEP_SLEEP_DURATION_S					4260	// maximum value is 0xFFFFFFFF=4294967295 (32 bits) µs which is about 71 minutes = 4260s
#elif defined (ESP32)
#	define DEEP_SLEEP_DURATION_S					7200	// = 2h, maximum value is 0xFFFFFFFFFFFFFFFF= (64 bits) µs
#endif

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class ModuleSequencer : public Module
{
	SINGLETON_CLASS(ModuleSequencer)

private:
	using fn_b = std::function <bool()>;

	unsigned long _shortDelayBetweenExecMs		= SHORT_DELAY_BETWEEN_EXEC_S * 1000;
	unsigned long _longDelayBetweenExecMs		= LONG_DELAY_BETWEEN_EXEC_S * 1000;
	unsigned long _delayBetweenWakeUpMs			= DELAY_BETWEEN_WAKEUP_S * 1000;

	unsigned long long _deepSleepDurationMs 	= DEEP_SLEEP_DURATION_S * 1000;
	fn_b 		  _checkCondToEnterDeepSleep	= [] { return false; };


	volatile unsigned long _lastWakeUpTimeStamp	= 0;				// We need to declare a variable as volatile when it can be changed unexpectedly
																	// (as in an ISR), so the compiler doesn’t remove it due to optimizations
	unsigned long _previousLoopersExecTimeStamp	= 0;

	bool _isWakeUpDelayOk						= false;
	bool _isTimeToReboot						= false;

	std::list <Module *> 						_modules;
	std::list <Module *> :: iterator 			_itModule;

private:

	bool isWakeUpRequested						();
	bool isTimeToEnterDeepSleep					() const;

public:

	Signal <bool> notifyWakeUpRequested;

public:

	void requestReboot							();
	void requestWakeUp		 					();

	void setShortDelayBetweenExecution			(unsigned long shortDelayBetweenExecMs)		{ _shortDelayBetweenExecMs	= shortDelayBetweenExecMs;	}
	void setLongDelayBetweenExecution			(unsigned long longDelayBetweenExecMs)		{ _longDelayBetweenExecMs	= longDelayBetweenExecMs;	}
	void setDelayBetweenWakeUp					(unsigned long delayBetweenWakeUpMs)		{ _delayBetweenWakeUpMs		= delayBetweenWakeUpMs;		}

	void setDeepSleepDuration					(unsigned long deepSleepDurationMs)			{ _deepSleepDurationMs		= deepSleepDurationMs;		}
	void setConditionToEnterDeepSleep			(fn_b checkCondToEnterDeepSleep)			{ _checkCondToEnterDeepSleep= checkCondToEnterDeepSleep;}
	void enterDeepSleepWhenWifiOff				();

	void setModules 							(std::list <Module *> modules);

	void setup									(std::list <Module *> modules);
	void loop									() override;
};













