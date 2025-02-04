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


#define SHORT_TIME_BETWEEN_MODULES_LOOP_ms			1000
#define LONG_TIME_BETWEEN_MODULES_LOOP_ms			10000

#define MIN_TIME_BETWEEN_WAKEUP_ms					60000

#ifdef ESP8266
#	define DEEP_SLEEP_TIME_S						4260	// maximum value is 0xFFFFFFFF=4294967295 (32 bits) µs which is about 71 minutes = 4260s
#elif defined (ESP32)
#	define DEEP_SLEEP_TIME_S						7200	// = 2h, maximum value is 0xFFFFFFFFFFFFFFFF= (64 bits) µs
#endif


namespace corex {

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class ModuleSequencer : public Module <const std::list <IModule *> &>
{
	SINGLETON_CLASS(ModuleSequencer)

private:
	using fn_b = std::function <bool()>;

	unsigned long long _deepSleepTimeMs 		= DEEP_SLEEP_TIME_S * 1000;

	fn_b 		  _isTimeToEnterDeepSleep		= [] { return false; };

	volatile bool _isWakeUpRequested			= false;	// We need to declare a variable as volatile when it can be changed unexpectedly
															// (as in an ISR), so the compiler doesn’t remove it due to optimizations
	unsigned long _lastWakeUpTimeStampMs		= -MIN_TIME_BETWEEN_WAKEUP_ms;
	unsigned long _lastModulesLoopTimeStampMs	= 0;

	bool _isRebootRequested						= false;

	std::list <IModule *> 						_modules;
	std::list <IModule *> :: iterator 			_itModule;

private:

	void setModules 							(const std::list <IModule *> & modules);

public:

	Signal <> notifyWakeUpRequested;

public:

	void requestReboot							();
	void requestWakeUp		 					();

	void setDeepSleepTime						(unsigned long deepSleepTimeMs)				{ _deepSleepTimeMs			= deepSleepTimeMs;			}
	void setConditionToEnterDeepSleep			(fn_b isTimeToEnterDeepSleep)				{ _isTimeToEnterDeepSleep	= isTimeToEnterDeepSleep;	}
	void enterDeepSleepWhenWifiOff				();

	void setup									(const std::list <IModule *> & modules) override;
	void loop									() override;
};

}











