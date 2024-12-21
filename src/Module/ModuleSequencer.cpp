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


#include "BoardDefs.h"
#include "Print/Logger.h"
#include "Storage/FileStorage.h"
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
	if (_sleepMode == SleepMode::DeepSleep) return true;
	return _checkCondToEnterDeepSleep ();
}

//========================================================================================================================
// ### DEEP-SLEEP ###
//========================================================================================================================
void ModuleSequencer :: enterDeepSleep () const
{
	Logln ("Enter in deep sleep mode..");

	if (!FileStorage::isFileExists(DEEPSLEEP_NAMEFILE)) {				// Si le fichier deepSleep.txt n'existe pas
		FileStorage::createFile(DEEPSLEEP_NAMEFILE);					// Creation du fichier deepsleep.txt
	}

	WiFiHelper::disconnectAll ();

#ifdef ESP8266

	// With ESP.deepSleep(0), esp will be going to sleep forever.
	ESP.deepSleep(_deepSleepDurationMs * 1000 /* µs */, WAKE_RF_DEFAULT /*WAKE_RF_DISABLED*/);	// WAKE_RF_DISABLED : this prevents the Wifi hardware from booting up after deep sleep

#elif defined (ESP32)

	esp_sleep_enable_timer_wakeup(_deepSleepDurationMs * 1000);
	esp_deep_sleep_start();

#endif
																								// Note that there is no way to enable it again without deep sleeping again
	delay(100);
}

//========================================================================================================================
//
//========================================================================================================================
void ModuleSequencer :: enterDeepSleepWhenWifiOff ()
{
	setConfitionToEnterDeepSleep ([] { return !WiFiHelper::isWifiAvailable(); } );
}

//========================================================================================================================
//
//========================================================================================================================
bool ModuleSequencer :: isDeviceWakeUpFromDeepSleep () const
{

#ifdef ESP8266

	rst_info *resetInfo;
	resetInfo = ESP.getResetInfoPtr();

	if (resetInfo->reason == REASON_DEEP_SLEEP_AWAKE) {
		if (FileStorage::isFileExists(DEEPSLEEP_NAMEFILE)) {
			return true;
		}
	}

#elif defined (ESP32)

	esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

	if ((wakeup_reason == ESP_SLEEP_WAKEUP_EXT0 ) ||
		(wakeup_reason == ESP_SLEEP_WAKEUP_EXT1 ) ||
		(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER ) ||
		(wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD ) ||
		(wakeup_reason == ESP_SLEEP_WAKEUP_ULP ))
	{
		if (FileStorage::isFileExists(DEEPSLEEP_NAMEFILE)) {
			return true;
		}
	}

#endif

	return false;
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
void ModuleSequencer :: setup (std::list <Module *> modules, SleepMode sleepMode /*= MODEM_SLEEP*/)
{
	_sleepMode = sleepMode;

	setModules (modules);

	if (WiFiHelper::isWifiAvailable()) {

		if (!_checkCondToEnterDeepSleep ()) {
			FileStorage::deleteFile (DEEPSLEEP_NAMEFILE);		// Efface le fichier "deepsleep.txt"
		}

		// *** Modem sleep & light sleep are just effective in station only mode ***
		if (!WiFiHelper::isAccessPointMode()) {
			if ((_sleepMode == SleepMode::AutoLightSleep) || (_sleepMode == SleepMode::ForcedLightSleep)) {

#ifdef ESP8266
				// Auto Light-sleep
				wifi_set_sleep_type(LIGHT_SLEEP_T);				// Just enable the function and let the system decide when it can sleep

#elif defined (ESP32)

				// https://mischianti.org/esp32-practical-power-saving-manage-wifi-and-cpu-1/

				// WIFI_PS_NONE: disable modem sleep entirely;
				// WIFI_PS_MIN_MODEM: enable Modem-sleep minimum power save mode;
				// WIFI_PS_MAX_MODEM: to enable Modem-sleep maximum power save mode.

				// WiFi.setSleep(false); // the power consumption changes dramaticalyl, we have set WIFI_PS_NONE
				// WiFi.setSleep(true); // defult parameter, so we set WIFI_PS_MIN_MODEM

				esp_wifi_set_ps (WIFI_PS_MAX_MODEM);

				// Another critical feature to reduce power consumption is cutting the CPU frequencies
				setCpuFrequencyMhz(/* The only value admitted : 240, 160, 80, 40, 20, 10 */ 40);
#endif

			}
		}
	}

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
				enterDeepSleep();
			}
			else {
				BLINK();
				_itModule = _modules.begin ();
			}
		}
		// millis takes 49+_days to rollover
		else if ((millis() < _previousLoopersExecTimeStamp) || _isTimeToReboot) {

			reboot ();
		}
	}
}
