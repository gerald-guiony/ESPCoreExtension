//************************************************************************************************************************
// EspBoard.cpp
// Version 1.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>
#include <HardwareSerial.h>

#ifdef ESP32
#	include <esp_task_wdt.h>
#	include <rom/rtc.h>
#endif

#include "BoardDefs.h"
#include "Print/Logger.h"
#include "Tools/Signal.h"
#include "Storage/FileStorage.h"
#include "WiFi/WiFiHelper.h"

#include "EspBoard.h"


#define DEEPSLEEP_NAMEFILE							"/deepsleep.txt"


#ifdef DEBUG
#	warning -- DEBUG defined --
#endif
#ifdef ESP8266
#	warning ** ESP8266 defined **
#endif
#ifdef ESP32
#	warning ** ESP32 defined **
#endif
#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
#	warning ** NODEMCU defined **
#endif
#ifdef ARDUINO_ESP8266_WIO_NODE
#	warning ** WIO_NODE defined **
#endif


//========================================================================================================================
//
//========================================================================================================================
void EspBoard :: init (bool enableDebugSerial /*= false */) {

	WiFiHelper::WiFiOff ();								// Save power during start up
	setPortPower (false);								// Power OFF the groove socket

#ifdef DEBUG

	if (enableDebugSerial) {

		Serial.begin(115200);							// change BAUD rate as required
		while (!Serial); 								// wait until Serial is established - required on some Platforms
		Serial.setDebugOutput (true);
		Serial << F("Serial is up") << LN;

		// if you wants serial echo - only recommended if ESP8266 is plugged in USB
	//	I(Logger).notifyRequestLineToPrint += std::bind (&HardwareSerial::print, &Serial, std::placeholders::_1);
		I(Logger).notifyRequestLineToPrint += [] (const String & line) { Serial.print (line); };

	}

	//	I(Logger).showTime (true);						// To show time
	//	I(Logger).showProfiler (true);					// To show profiler - time between messages of Debug - Good to "begin ...." and "end ...." messages
	//	I(Logger).showColors (false);					// Colors
		I(Logger).showChipName (true);					// Name of this Esp8266

	//	delay (10000);									// Delay before starting

#endif

#ifdef BLINKLED
	pinMode (BLINKLED, OUTPUT);							// Set led pin as output
#endif

	Logln (F("\n\n******* Chip is (re)booting *******"));

	FileStorage::init ();								// Init file system

	if (!isWakeUpFromDeepSleep ()) {
		FileStorage::deleteFile (DEEPSLEEP_NAMEFILE);	// Efface le fichier "deepsleep.txt"
	}
}


//========================================================================================================================
//
//========================================================================================================================
void EspBoard :: reboot () {

#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	pinMode (D0, OUTPUT);								// Nécessaire quand GPIO16 est relié au RST pin (DeepSleep)
	digitalWrite (D0, LOW);
#endif

	asyncDelayMillis (2000);
	ESP.restart();
	asyncDelayMillis (2000);
}

//========================================================================================================================
//
//========================================================================================================================
const String EspBoard :: getDeviceMemoryStats () {
	StreamString mem;
	mem << F("Free ram memory = ") << ESP.getFreeHeap() << F(" bytes") << LN;
	return mem;
}

//========================================================================================================================
//
//========================================================================================================================
uint32_t EspBoard :: getDeviceId () {

#ifdef ESP8266

	return ESP.getChipId();

#elif defined (ESP32)

	uint32_t chipId = 0;
	for (int i = 0; i < 17; i = i + 8) {
		chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
	return chipId;

#endif
}

//========================================================================================================================
//
//========================================================================================================================
const String EspBoard :: getDeviceName () {
	static StreamString name;
	if (name.length() == 0) {
		name << F("ESP") << getDeviceId ();
	}
	return name;
}

//========================================================================================================================
//
//========================================================================================================================
void EspBoard :: setPortPower (bool on) {

#ifdef ARDUINO_ESP8266_WIO_NODE
	// The power supply of Grove sockets is controlled by a MOSFET switch which is gated by GPIO 15. So you must pull up
	// GPIO 15 in your Arduino sketch to power on the Grove system
	pinMode (PORT_POWER, OUTPUT);
	digitalWrite (PORT_POWER, on ? HIGH : LOW);
#endif

	//WiFi.setOutputPower(on ? 20.5 : 0.0); // (dBm	max: +20.5dBm min: 0dBm) sets transmit power to 0dbm to lower power consumption, but reduces usable range
}

//========================================================================================================================
// Dans le cas d'une grosse attente la methode native delay(ms) peut déclencher le watchdog reset
// Cette méthode permet également de remplacer la méthode delay(ms) dans les callbacks async  (ESPAsyncWebServer)
//========================================================================================================================
void EspBoard :: asyncDelayMillis (unsigned int ms) {

#if defined (ESP8266)
	ESP.wdtFeed();							// Explicitly restart the software watchdog
//	yield();
#elif defined (ESP32)
	esp_task_wdt_reset();
#endif

	unsigned int loop = 0;
	unsigned long time = ms * 1000;			// En microseconds..

	while (time > 16383) {					// delayMicroseconds is only accurate to 16383us.
		delayMicroseconds (16383);
		time -= 16383;
		loop ++;
		if (loop % 10 == 0) {
#if defined (ESP8266)
			ESP.wdtFeed();					// Every 163 ms, the ESP wdt is fed and the cpu is yielded for any pending tasks.
//			yield();						// This should eliminate watchdog timer soft resets
#elif defined (ESP32)
			esp_task_wdt_reset();
#endif
		}
	}
	delayMicroseconds (time);
}

//========================================================================================================================
//
//========================================================================================================================
const String EspBoard :: getResetReason(int cpuNo) {

#if defined (ESP8266)

	return ESP.getResetReason ();

#elif defined (ESP32)

	switch (rtc_get_reset_reason(cpuNo)) {
		case 1:  return F("Vbat power on reset"); break;
		case 3:  return F("Software reset digital core"); break;
		case 4:  return F("Legacy watch dog reset digital core"); break;
		case 5:  return F("Deep Sleep reset digital core"); break;
		case 6:  return F("Reset by SLC module, reset digital core"); break;
		case 7:  return F("Timer Group0 Watch dog reset digital core"); break;
		case 8:  return F("Timer Group1 Watch dog reset digital core"); break;
		case 9:  return F("RTC Watch dog Reset digital core"); break;
		case 10: return F("Instrusion tested to reset CPU"); break;
		case 11: return F("Time Group reset CPU"); break;
		case 12: return F("Software reset CPU"); break;
		case 13: return F("RTC Watch dog Reset CPU"); break;
		case 14: return F("for APP CPU, reset by PRO CPU"); break;
		case 15: return F("Reset when the vdd voltage is not stable"); break;
		case 16: return F("RTC Watch dog reset digital core and rtc module"); break;
		default: return F("NO_MEAN");
	}

	return "";

#endif
}

//========================================================================================================================
//
//========================================================================================================================
const String EspBoard :: getTimeElapsedSinceBoot () {

	unsigned long runMillis = millis();
	unsigned long allSeconds = runMillis / 1000;

	int runHours = allSeconds / 3600;
	int secsRemaining = allSeconds % 3600;
	int runMinutes = secsRemaining / 60;
	int runSeconds = secsRemaining % 60;

	char buf [20];
	sprintf (buf,"%02dh%02dm%02ds", runHours, runMinutes, runSeconds);

	return buf;
}

//========================================================================================================================
//
//========================================================================================================================
int EspBoard :: getPinMode (uint8_t pin) {

  if (pin >= NUM_DIGITAL_PINS) return (-1);

  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint32_t *reg = portModeRegister(port);
  if (*reg & bit) return (OUTPUT);

  volatile uint32_t *out = portOutputRegister(port);
  return ((*out & bit) ? INPUT_PULLUP : INPUT);
}

//========================================================================================================================
//
//========================================================================================================================
void EspBoard :: blinkOn () {
#if defined(BLINKLED)
	if (EspBoard::getPinMode(BLINKLED) == OUTPUT) {
		digitalWrite(BLINKLED, LOW);					// Inverted logic !!
	}
#else
	Log(F(".");
#endif
}

//========================================================================================================================
//
//========================================================================================================================
void EspBoard :: blinkOff () {
#if defined(BLINKLED)
	if (EspBoard::getPinMode(BLINKLED) == OUTPUT) {
		digitalWrite(BLINKLED, HIGH);
	}
#endif
}

//========================================================================================================================
//
//========================================================================================================================
void EspBoard :: blink () {
	blinkOn();
	asyncDelayMillis (5);
	blinkOff();
}

//========================================================================================================================
//
//========================================================================================================================
void EspBoard :: blinks (int nbBlink) {
	do {blink(); asyncDelayMillis (100); nbBlink--;} while (nbBlink > 0);
}

//========================================================================================================================
// ### DEEP-SLEEP ###
//========================================================================================================================
void EspBoard :: enterDeepSleep (unsigned long deepSleepDurationMs) {

	Logln ("Enter in deep sleep mode..");

	if (!FileStorage::isFileExists(DEEPSLEEP_NAMEFILE)) {				// Si le fichier deepSleep.txt n'existe pas
		FileStorage::createFile(DEEPSLEEP_NAMEFILE);					// Creation du fichier deepsleep.txt
	}

	WiFiHelper::disconnectAll ();

#ifdef ESP8266

	// With ESP.deepSleep(0), esp will be going to sleep forever.
	ESP.deepSleep(deepSleepDurationMs * 1000 /* µs */, WAKE_RF_DEFAULT /*WAKE_RF_DISABLED*/);	// WAKE_RF_DISABLED : this prevents the Wifi hardware from booting up after deep sleep

#elif defined (ESP32)

	esp_sleep_enable_timer_wakeup(deepSleepDurationMs * 1000);
	esp_deep_sleep_start();

#endif
																								// Note that there is no way to enable it again without deep sleeping again
	delay(100);
}

//========================================================================================================================
//
//========================================================================================================================
bool EspBoard :: isWakeUpFromDeepSleep () {

#ifdef ESP8266

	rst_info *resetInfo;
	resetInfo = ESP.getResetInfoPtr();

	if (resetInfo->reason == REASON_DEEP_SLEEP_AWAKE) {
		return (FileStorage::isFileExists(DEEPSLEEP_NAMEFILE));
	}

#elif defined (ESP32)

	esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

	if ((wakeup_reason == ESP_SLEEP_WAKEUP_EXT0 ) ||
		(wakeup_reason == ESP_SLEEP_WAKEUP_EXT1 ) ||
		(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER ) ||
		(wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD ) ||
		(wakeup_reason == ESP_SLEEP_WAKEUP_ULP ))
	{
		return (FileStorage::isFileExists(DEEPSLEEP_NAMEFILE));
	}

#endif

	return false;
}

//========================================================================================================================
// Very risky !
//========================================================================================================================
void EspBoard :: disableHardwareWatchdog () {
#ifdef ESP8266
  *((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
#endif
}

//========================================================================================================================
//
//========================================================================================================================
void EspBoard :: enableHardwareWatchdog () {
#ifdef ESP8266
  *((volatile uint32_t*) 0x60000900) |= 1; // Hardware WDT ON
#endif
}