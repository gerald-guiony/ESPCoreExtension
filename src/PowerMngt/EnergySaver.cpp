//************************************************************************************************************************
// EnergySaver.cpp
// Version 1.0 June, 2017
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

#include "EnergySaver.h"


SINGLETON_IMPL (EnergySaver)


//========================================================================================================================
//
//========================================================================================================================
void EnergySaver :: setModulesPower (bool on)
{
#ifdef ARDUINO_ESP8266_WIO_NODE
	// The power supply of Grove sockets is controlled by a MOSFET switch which is gated by GPIO 15. So you must pull up
	// GPIO 15 in your Arduino sketch to power on the Grove system
	pinMode (PORT_POWER, OUTPUT);
	digitalWrite (PORT_POWER, on ? HIGH : LOW);
#endif

	//WiFi.setOutputPower(on ? 20.5 : 0.0); // (dBm	max: +20.5dBm min: 0dBm) sets transmit power to 0dbm to lower power consumption, but reduces usable range
}

//========================================================================================================================
//
//========================================================================================================================
void EnergySaver ::requestReboot ()
{
	_requestReboot = true;
}

//========================================================================================================================
//
//========================================================================================================================
void EnergySaver :: wakeUp ()
{
	BLINK();
	_lastWakeUpTimeStamp = millis();
	
	Logln (getChipMemoryStats());
}

//========================================================================================================================
//
//========================================================================================================================
void EnergySaver :: updateWakeUpState ()
{
	bool wakeUpState = millis() - _lastWakeUpTimeStamp <= WAKEUP_DURATION_MS;
	if (wakeUpState != _wakeUpState) {
		_wakeUpState = wakeUpState;
		notifyWakeUpStateChanged (wakeUpState);
	}
}

//========================================================================================================================
//
//========================================================================================================================
bool EnergySaver :: isDeepSleepTimeSlot () const
{
	if (_sleepMode == SleepMode::DeepSleep) {
		return true;
	}
	else {
		if (_enterDeepSleepOnWifiOff)
		{
			if(!WiFiHelper::isWifiAvailable()) {						// Si plus ou pas de wifi disponible
				return (true);
			}
		}
		return false;
	}
}

//========================================================================================================================
// ### DEEP-SLEEP ###
//========================================================================================================================
void EnergySaver :: enterInDeepSleep ()
{
	Logln ("Enter in deep sleep mode..");
	
	unsigned long deepSleepDuration = DEEP_SLEEP_DURATION_MS;			// maximum value is 0xFFFFFFFF=4294967295 (32 bits) us which is about 71 minutes
	
	if (!FileStorage::isFileExists(DEEPSLEEP_NAMEFILE)) {				// Si le fichier deepSleep.txt n'existe pas
		if (_sleepMode != SleepMode::DeepSleep) {
			deepSleepDuration = FIRST_DEEP_SLEEP_DURATION_MS;
		}
		FileStorage::createFile(DEEPSLEEP_NAMEFILE);					// Creation du fichier deepsleep.txt
	}
	
	WiFiHelper::disconnectAll ();

	// With ESP.deepSleep(0), esp will be going to sleep forever.
	ESP.deepSleep(deepSleepDuration * 1000 /* µs */, WAKE_RF_DEFAULT /*WAKE_RF_DISABLED*/);	// WAKE_RF_DISABLED : this prevents the Wifi hardware from booting up after deep sleep	
																							// Note that there is no way to enable it again without deep sleeping again 
	delay(100);
}

//========================================================================================================================
//
//========================================================================================================================
bool EnergySaver :: isWakeUpFromDeepSleep () const
{
	rst_info *resetInfo;
	resetInfo = ESP.getResetInfoPtr();
	
	if (resetInfo->reason == REASON_DEEP_SLEEP_AWAKE) {
		if (FileStorage::isFileExists(DEEPSLEEP_NAMEFILE)) {	// Ceinture et bretelles :)
			Logln ("Wake up after deep sleep mode..");
			return true;
		}
	}
	return false;
}

//========================================================================================================================
//
//========================================================================================================================
void EnergySaver :: setLoopers (std::list <Looper *> loopers)
{
	_loopers.assign(loopers.begin(), loopers.end());
	_itLooper = _loopers.end();
}

//========================================================================================================================
//
//========================================================================================================================
void EnergySaver :: setup (std::list <Looper *> loopers, SleepMode sleepMode /*= MODEM_SLEEP*/)
{
	_sleepMode = sleepMode;
	
	setModulesPower (false);

	setLoopers (loopers);
		
	if (WiFiHelper::isWifiAvailable()) {
		
		if (_enterDeepSleepOnWifiOff) {
			FileStorage::deleteFile(DEEPSLEEP_NAMEFILE);		// Effacer le fichier deepsleep.txt
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
	
	_lastRunLoopTimeStamp = millis ();
}

//========================================================================================================================
//
//========================================================================================================================
void EnergySaver :: loop ()
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
			_lastRunLoopTimeStamp = millis ();
		}
	}
	else {

		updateWakeUpState ();

		unsigned long delayBeforeNextRunLoop = _wakeUpState ? SHORT_RUNLOOP_DELAY_MS : LONG_RUNLOOP_DELAY_MS;

		if ((millis() - _lastRunLoopTimeStamp > delayBeforeNextRunLoop)) {

			if (isDeepSleepTimeSlot ()) {
				enterInDeepSleep();
			}
			else {
				BLINK();
				_itLooper = _loopers.begin ();
			}
		}
		// millis takes 49+_days to rollover
		else if ((millis() < _lastRunLoopTimeStamp) || _requestReboot) {

			reboot ();
		}
	}
}
