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


#define SHORT_TIME_BETWEEN_EXEC_S					1
#define LONG_TIME_BETWEEN_EXEC_S					10

#define TIME_BETWEEN_WAKEUP_S						60

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

	unsigned long _shortTimeBetweenExecMs		= SHORT_TIME_BETWEEN_EXEC_S * 1000;
	unsigned long _longTimeBetweenExecMs		= LONG_TIME_BETWEEN_EXEC_S * 1000;
	unsigned long _timeBetweenWakeUpMs			= TIME_BETWEEN_WAKEUP_S * 1000;

	unsigned long long _deepSleepTimeMs 		= DEEP_SLEEP_TIME_S * 1000;
	fn_b 		  _isTimeToEnterDeepSleep		= [] { return false; };


	volatile unsigned long _lastWakeUpTimeStamp	= 0;		// We need to declare a variable as volatile when it can be changed unexpectedly
															// (as in an ISR), so the compiler doesn’t remove it due to optimizations
	unsigned long _previousModulesExecTimeStamp	= 0;

	bool _isWakeUpTimeOk						= false;
	bool _isTimeToReboot						= false;

	std::list <IModule *> 						_modules;
	std::list <IModule *> :: iterator 			_itModule;

private:

	bool isWakeUpRequested						();
	void setModules 							(const std::list <IModule *> & modules);

public:

	Signal <bool> notifyWakeUpRequested;

public:

	void requestReboot							();
	void requestWakeUp		 					();

	void setShortTimeBetweenExecution			(unsigned long shortTimeBetweenExecMs)		{ _shortTimeBetweenExecMs	= shortTimeBetweenExecMs;	}
	void setLongTimeBetweenExecution			(unsigned long longTimeBetweenExecMs)		{ _longTimeBetweenExecMs	= longTimeBetweenExecMs;	}
	void setTimeBetweenWakeUp					(unsigned long timeBetweenWakeUpMs)			{ _timeBetweenWakeUpMs		= timeBetweenWakeUpMs;		}

	void setDeepSleepTime						(unsigned long deepSleepTimeMs)				{ _deepSleepTimeMs			= deepSleepTimeMs;			}
	void setConditionToEnterDeepSleep			(fn_b isTimeToEnterDeepSleep)				{ _isTimeToEnterDeepSleep	= isTimeToEnterDeepSleep;	}
	void enterDeepSleepWhenWifiOff				();

	void setup									(const std::list <IModule *> & modules) override;
	void loop									() override;
};

}











