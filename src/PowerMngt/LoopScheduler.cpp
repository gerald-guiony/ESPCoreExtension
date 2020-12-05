//************************************************************************************************************************
// LoopScheduler.cpp
// Version 2.0 June, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#ifdef ESP8266
#	include  <ESP8266WiFi.h>
	extern "C" {
		#include "user_interface.h"
	}
#endif

#include "BoardDefs.h"
#include "Print/Logger.h"
#include "Storage/FileStorage.h"
#include "WiFi/WiFiHelper.h"

#include "LoopScheduler.h"


SINGLETON_IMPL (LoopScheduler)


//========================================================================================================================
//
//========================================================================================================================
void LoopScheduler ::requestReboot ()
{
	_requestReboot = true;
}

//========================================================================================================================
//
//========================================================================================================================
void LoopScheduler :: wakeUp ()
{
	_lastWakeUpTimeStamp = millis();
}

//========================================================================================================================
//
//========================================================================================================================
bool LoopScheduler :: updateWakeUpState ()
{
	bool wakeUpState = millis() - _lastWakeUpTimeStamp <= _wakeUpDurationMs;
	if (wakeUpState != _wakeUpState) {
		_wakeUpState = wakeUpState;
		notifyWakeUpStateChanged (wakeUpState);	
		return true;
	}
	return false;
}

//========================================================================================================================
//
//========================================================================================================================
bool LoopScheduler :: isOkToEnterDeepSleep () const
{
	if (_sleepMode == SleepMode::DeepSleep) return true;
	return _isOkToDeepSleep ();
}

//========================================================================================================================
// ### DEEP-SLEEP ###
//========================================================================================================================
void LoopScheduler :: enterDeepSleep () const
{
	Logln ("Enter in deep sleep mode..");
	
	if (!FileStorage::isFileExists(DEEPSLEEP_NAMEFILE)) {				// Si le fichier deepSleep.txt n'existe pas
		FileStorage::createFile(DEEPSLEEP_NAMEFILE);					// Creation du fichier deepsleep.txt
	}
	
	WiFiHelper::disconnectAll ();

	// With ESP.deepSleep(0), esp will be going to sleep forever.
	ESP.deepSleep(_deepSleepDurationMs * 1000 /* µs */, WAKE_RF_DEFAULT /*WAKE_RF_DISABLED*/);	// WAKE_RF_DISABLED : this prevents the Wifi hardware from booting up after deep sleep	
																								// Note that there is no way to enable it again without deep sleeping again 
	delay(100);
}

//========================================================================================================================
//
//========================================================================================================================
void LoopScheduler :: enterDeepSleepIfWifiOff ()
{
	enterDeepSleepIf ([] { return !WiFiHelper::isWifiAvailable(); } );
}

//========================================================================================================================
//
//========================================================================================================================
bool LoopScheduler :: isWakeUpFromDeepSleep () const
{
	rst_info *resetInfo;
	resetInfo = ESP.getResetInfoPtr();
	
	if (resetInfo->reason == REASON_DEEP_SLEEP_AWAKE) {
		if (FileStorage::isFileExists(DEEPSLEEP_NAMEFILE)) {
			return true;
		}
	}
	return false;
}

//========================================================================================================================
//
//========================================================================================================================
void LoopScheduler :: setLoopers (std::list <Looper *> loopers)
{
	_loopers.assign(loopers.begin(), loopers.end());
	_itLooper = _loopers.end();
}

//========================================================================================================================
//
//========================================================================================================================
void LoopScheduler :: setup (std::list <Looper *> loopers, SleepMode sleepMode /*= MODEM_SLEEP*/)
{
	_sleepMode = sleepMode;
	
	setLoopers (loopers);
		
	if (WiFiHelper::isWifiAvailable()) {
		
		if (!_isOkToDeepSleep ()) {
			FileStorage::deleteFile (DEEPSLEEP_NAMEFILE);		// Effacer le fichier deepsleep.txt
		}
		
		// *** Modem sleep & light sleep are just effective in station only mode *** 
		if (!WiFiHelper::isAccessPointMode()) {
			if ((_sleepMode == SleepMode::AutoLightSleep) || (_sleepMode == SleepMode::ForcedLightSleep)) {
				// Auto Light-sleep
				wifi_set_sleep_type(LIGHT_SLEEP_T);				// Just enable the function and let the system decide when it can sleep
			}
		}
	}

	wakeUp ();
	
	_lastExecTimeStamp = millis ();
}

//========================================================================================================================
//
//========================================================================================================================
void LoopScheduler :: loop ()
{
	// The watchdog timer is automatically reset when loop() returns !
	// Call yield() or delay() periodically, both will indirectly reset the watchdog.
	// yield() function is equivalent to delay(0)
	// Explicitly restart the software watchdog
	//ESP.wdtFeed ();

	if (_itLooper != _loopers.end ()) {

		// The ESP8266 runs a lot of utility functions in the background – keeping WiFi connected, managing the TCP/IP stack, and performing other duties. Blocking these 
		// functions from running can cause the ESP8266 to crash and reset itself. To avoid these mysterious resets, avoid long, blocking loops in your sketch.
		(*_itLooper)->loop ();

		_itLooper++;

		if (_itLooper == _loopers.end ()) {
			_lastExecTimeStamp = millis ();
		}
	}
	else {

		unsigned long delayBeforeNextExec = _wakeUpState ? _shortDelayBetweenExecMs : _longDelayBetweenExecMs;

		if ((millis() - _lastExecTimeStamp > delayBeforeNextExec) || updateWakeUpState ()) {

			if (isOkToEnterDeepSleep ()) {
				enterDeepSleep();
			}
			else {
				BLINK();
				_itLooper = _loopers.begin ();
			}
		}
		// millis takes 49+_days to rollover
		else if ((millis() < _lastExecTimeStamp) || _requestReboot) {

			reboot ();
		}
	}
}
