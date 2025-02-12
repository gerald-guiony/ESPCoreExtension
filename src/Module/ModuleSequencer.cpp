//************************************************************************************************************************
// ModuleSequencer.cpp
// Version 2.0 June, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>

#include "EspBoard.h"
#include "Print/Logger.h"
#include "BlinkerModule.h"
#include "WiFi/WiFiHelper.h"

#include "ModuleSequencer.h"


namespace corex {


SINGLETON_IMPL (ModuleSequencer)


//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: requestReboot ()
{
	_isRebootRequested = true;
}

//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: requestWakeUp ()
{
	_isWakeUpRequested = true;
}

//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: enterDeepSleepWhenWifiOff ()
{
	setConditionToEnterDeepSleep ([] { return !WiFiHelper::isWifiAvailable(); } );
}

//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: setModules (const std::list <IModule *> & modules, bool addBlinkerModule)
{
	_modules = modules;
	if (addBlinkerModule) {
		_modules.push_back(&I(BlinkerModule));
	}
	_itModule = _modules.begin ();
}

//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: setup (const std::list <IModule *> & modules, bool addBlinkerModule)
{
	setModules (modules, addBlinkerModule);
	requestWakeUp ();
}

//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: loop ()
{
	// The watchdog timer is automatically reset when loop() returns !
	// Call yield() or delay() periodically, both will indirectly reset the watchdog.
	// yield() function is equivalent to delay(0)
	// Explicitly restart the software watchdog
	//ESP.wdtFeed ();

	if (_itModule != _modules.end ()) {

		// The ESP8266 runs a lot of utility functions in the background – keeping WiFi connected, managing the TCP/IP stack, and performing other duties. Blocking these
		// functions from running can cause the ESP8266 to crash and reset itself. To avoid these mysterious resets, avoid long, blocking loops in your sketch.
		(*_itModule)->loop ();

		_itModule++;

		if (_itModule == _modules.end ()) {
			_lastModulesLoopTimeStampMs = millis ();
		}
	}
	else {

		if (_isTimeToEnterDeepSleep ()) {
			EspBoard::enterDeepSleep(_deepSleepTimeMs);
		}
		// millis takes 49+_days to rollover
		else if ((millis() < _lastModulesLoopTimeStampMs) || _isRebootRequested) {
			EspBoard::reboot ();
		}

		static bool previousIsAwake = false;
		bool isAwake = (millis() - _lastWakeUpTimeStampMs <= MIN_TIME_BETWEEN_WAKEUP_ms);

		if (_isWakeUpRequested) {
			_isWakeUpRequested = false;
			_lastWakeUpTimeStampMs = millis();
			isAwake = true;
		}

		if (previousIsAwake != isAwake) {
			notifyAwake (isAwake);
			previousIsAwake = isAwake;
		}

		unsigned long timeBeforeNextModulesLoopMs = isAwake ? SHORT_TIME_BETWEEN_MODULES_LOOP_ms : LONG_TIME_BETWEEN_MODULES_LOOP_ms;

		if (millis() - _lastModulesLoopTimeStampMs > timeBeforeNextModulesLoopMs) {
			_itModule = _modules.begin ();
		}
	}
}

}