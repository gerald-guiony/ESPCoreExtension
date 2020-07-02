//************************************************************************************************************************
// Global.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>
#include <HardwareSerial.h>

#include "BoardDefs.h"
#include "Print/Logger.h"
#include "Tools/Delegate.h"
#include "Storage/FileStorage.h"
#include "WiFi/WiFiHelper.h"

#include "Global.h"



#ifdef DEBUG
#	warning -- DEBUG defined --
#endif
#ifdef ESP8266
#	warning ** ESP8266 defined **
#endif
#ifdef ARDUINO_ESP8266_NODEMCU
#	warning ** NODEMCU defined **
#endif
#ifdef ARDUINO_ESP8266_WIO_NODE
#	warning ** WIO_NODE defined **
#endif



#ifdef ESP8266

//========================================================================================================================
//
//========================================================================================================================
void initSketch (bool enableDebugSerial /*= true */) {

	WiFiHelper::WiFiOff ();								// Save power during start up

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
String getChipName () {
	static StreamString name;
	if (name.length() == 0) {
		name << F("ESP") << ESP.getChipId();
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
// Dans le cas d'une grosse attente la methode native delay(ms) peut déclencher le watchdog reset
// Cette méthode permet également de remplacer la méthode delay(ms) dans les callbacks async  (ESPAsyncWebServer)
//========================================================================================================================
void asyncDelayMillis (unsigned int ms) {
	
	ESP.wdtFeed();							// Explicitly restart the software watchdog
//	yield();
	
	unsigned int loop = 0;
	unsigned long time = ms * 1000;			// En microseconds..
	
	while (time > 16383) {					// delayMicroseconds is only accurate to 16383us.
		delayMicroseconds (16383);
		time -= 16383;
		loop ++;
		if (loop % 10 == 0) {
			ESP.wdtFeed();					// Every 163 ms, the ESP wdt is fed and the cpu is yielded for any pending tasks.
//			yield();						// This should eliminate watchdog timer soft resets
		}
	}
	delayMicroseconds (time);
}

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