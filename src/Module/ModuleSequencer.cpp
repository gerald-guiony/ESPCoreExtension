//************************************************************************************************************************
// ModuleSequencer.cpp
// Version 2.0 June, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>

#include "EspBoard.h"
#include "Print/Logger.h"
#include "WiFi/WiFiHelper.h"

#include "ModuleSequencer.h"


SINGLETON_IMPL (ModuleSequencer)


//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer ::requestReboot ()
{
	_isTimeToReboot = true;
}

//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: requestWakeUp ()
{
	_lastWakeUpTimeStamp = millis();
}

//========================================================================================================================
//
//========================================================================================================================
bool ModuleSequencer :: isWakeUpRequested ()
{
	bool isWakeUpTimeOk = (millis() - _lastWakeUpTimeStamp <= _timeBetweenWakeUpMs);
	if (isWakeUpTimeOk != _isWakeUpTimeOk) {
		_isWakeUpTimeOk = isWakeUpTimeOk;
		notifyWakeUpRequested (isWakeUpTimeOk);
		return true;
	}
	return false;
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
void ModuleSequencer :: setModules (std::list <Module *> modules)
{
	_modules.assign(modules.begin(), modules.end());
	_itModule = _modules.end();
}

//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: setup (std::list <Module *> modules)
{
	setModules (modules);
	requestWakeUp ();
	_previousModulesExecTimeStamp = millis ();
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
			_previousModulesExecTimeStamp = millis ();
		}
	}
	else {

		unsigned long timeBeforeNextExec = _isWakeUpTimeOk ? _shortTimeBetweenExecMs : _longTimeBetweenExecMs;

		if ((millis() - _previousModulesExecTimeStamp > timeBeforeNextExec) || isWakeUpRequested ()) {

			if (_isTimeToEnterDeepSleep ()) {
				EspBoard::enterDeepSleep(_deepSleepTimeMs);
			}
			else {
				EspBoard::blink();
				_itModule = _modules.begin ();
			}
		}
		// millis takes 49+_days to rollover
		else if ((millis() < _previousModulesExecTimeStamp) || _isTimeToReboot) {
			EspBoard::reboot ();
		}
	}
}
