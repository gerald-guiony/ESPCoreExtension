//************************************************************************************************************************
// Global.cpp
// Version 1.0 June, 2017
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

#include "Global.h"



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



#if defined (ESP8266) || defined (ESP32)

//========================================================================================================================
//
//========================================================================================================================
void initSketch (bool enableDebugSerial /*= false */) {

	WiFiHelper::WiFiOff ();								// Save power during start up
	setModulesPower (false);							// Power OFF the groove socket

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

	if (BLINKLED >= 0) pinMode (BLINKLED, OUTPUT);		// Set led pin as output

	Logln (F("\n\n******* Chip is (re)booting *******"));

	FileStorage::init ();								// Init file system
}


//========================================================================================================================
//
//========================================================================================================================
void reboot () {
	pinMode (16, OUTPUT);								// Nécessaire quand GPIO16 est relié au RST pin (DeepSleep)
	digitalWrite (16, LOW);

	asyncDelayMillis (2000);
	ESP.restart();
	asyncDelayMillis (2000);
}

//========================================================================================================================
//
//========================================================================================================================
String getChipMemoryStats () {
	StreamString mem;
	mem << F("Free ram memory = ") << ESP.getFreeHeap() << F(" bytes") << LN;
	return mem;
}


//========================================================================================================================
//
//========================================================================================================================
uint32_t getChipId () {

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
String getChipName () {
	static StreamString name;
	if (name.length() == 0) {
		name << F("ESP") << getChipId ();
	}
	return name;
}

//========================================================================================================================
//
//========================================================================================================================
void setModulesPower (bool on)
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
// Dans le cas d'une grosse attente la methode native delay(ms) peut déclencher le watchdog reset
// Cette méthode permet également de remplacer la méthode delay(ms) dans les callbacks async  (ESPAsyncWebServer)
//========================================================================================================================
void asyncDelayMillis (unsigned int ms) {

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
const String get_reset_reason(int cpuNo) {

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
void disableHardwareWatchdog () {
  *((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
}

//========================================================================================================================
//
//========================================================================================================================
void enableHardwareWatchdog () {
  *((volatile uint32_t*) 0x60000900) |= 1; // Hardware WDT ON
}

#else

//========================================================================================================================
//
//========================================================================================================================
void initSketch (bool enableDebugSerial /*= true */) {

#ifdef DEBUG

	if (enableDebugSerial) {
		Serial.begin(115200);							// change BAUD rate as required
		while (!Serial); 								// wait until Serial is established - required on some Platforms
		Serial.setDebugOutput (true);
		Serial << F("Serial is up") << LN;
	}

#endif

	Logln (F("******* Chip is (re)booting *******"));
}

//========================================================================================================================
//
//========================================================================================================================
void reboot () {
}

//========================================================================================================================
//
//========================================================================================================================
String getChipMemoryStats () {

	extern int __heap_start, *__brkval;
	int v;
	uint32_t free = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

	StreamString mem;
	mem << F("Free ram memory = ")	<< free	<< F(" bytes") << LN;
	return mem;
}

//========================================================================================================================
//
//========================================================================================================================
String getChipName () {
	return (F("Arduino"));
}

#endif


//========================================================================================================================
//
//========================================================================================================================
String timeElapsedSinceBoot () {

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
bool strToLong (const String & str, long & value) {

	if (str.length () <= 0) return false;
	if ((str[0] != '-') && !isDigit (str[0])) return false;
	if ((str[0] == '-') && (str.length ()<=1)) return false;

	for (int i=1; i<str.length (); i++) {
		if (!isdigit (str [i])) return false;
	}

	value = str.toInt();
	return true;
}

//========================================================================================================================
//
//========================================================================================================================
void blinkStatus (int nbBlink) {
	do {BLINK(); asyncDelayMillis (100); nbBlink--;} while (nbBlink > 0);
}

//========================================================================================================================
//
//========================================================================================================================
int getPinMode (uint8_t pin)
{
  if (pin >= NUM_DIGITAL_PINS) return (-1);

  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint32_t *reg = portModeRegister(port);
  if (*reg & bit) return (OUTPUT);

  volatile uint32_t *out = portOutputRegister(port);
  return ((*out & bit) ? INPUT_PULLUP : INPUT);
}