//************************************************************************************************************************
// ModuleSequencer.cpp
// Version 2.0 June, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#ifdef ESP8266
#	include  <ESP8266WiFi.h>
	extern "C" {
		#include "user_interface.h"
	}
#elif defined(ESP32)
#	include <WiFi.h>
#	include <esp_wifi.h>
#endif


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
	bool isWakeUpDelayOk = (millis() - _lastWakeUpTimeStamp <= _delayBetweenWakeUpMs);
	if (isWakeUpDelayOk != _isWakeUpDelayOk) {
		_isWakeUpDelayOk = isWakeUpDelayOk;
		notifyWakeUpRequested (isWakeUpDelayOk);
		return true;
	}
	return false;
}

//========================================================================================================================
//
//========================================================================================================================
bool ModuleSequencer :: isTimeToEnterDeepSleep () const
{
	return _checkCondToEnterDeepSleep ();
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

	_previousLoopersExecTimeStamp = millis ();
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
			_previousLoopersExecTimeStamp = millis ();
		}
	}
	else {

		unsigned long delayBeforeNextExec = _isWakeUpDelayOk ? _shortDelayBetweenExecMs : _longDelayBetweenExecMs;

		if ((millis() - _previousLoopersExecTimeStamp > delayBeforeNextExec) || isWakeUpRequested ()) {

			if (isTimeToEnterDeepSleep ()) {
				EspBoard::enterDeepSleep(_deepSleepDurationMs);
			}
			else {
				EspBoard::blink();
				_itModule = _modules.begin ();
			}
		}
		// millis takes 49+_days to rollover
		else if ((millis() < _previousLoopersExecTimeStamp) || _isTimeToReboot) {
			EspBoard::reboot ();
		}
	}
}
