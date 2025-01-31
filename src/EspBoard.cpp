//************************************************************************************************************************
// EspBoard.cpp
// Version 1.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>
#include <HardwareSerial.h>

#ifdef ESP32
#	include <esp_system.h>
#	include <esp_task_wdt.h>
#	include <esp_err.h>
#	include <esp_wifi.h>
#	include <rom/rtc.h>
#endif

#include "EspBoardDefs.h"
#include "Print/Logger.h"
#include "Tools/Signal.h"
#include "Storage/FileStorage.h"
#include "WiFi/WiFiHelper.h"

#include "EspBoard.h"


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
#ifdef ARDUINO_WT32_ETH01
#	warning ** WT32_ETH01 defined **
#endif

namespace corex {

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
	yield();
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
			yield();
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
void EspBoard :: enterDeepSleep (unsigned long long deepSleepTimeMs) {

	Logln ("Enter in deep sleep mode..");

	WiFiHelper::disconnectAll ();

#ifdef ESP8266

	// With ESP.deepSleep(0), esp will be going to sleep forever.
	ESP.deepSleep(uint32_t(1000) * deepSleepTimeMs /* µs */, WAKE_RF_DEFAULT /*WAKE_RF_DISABLED*/);	// WAKE_RF_DISABLED : this prevents the Wifi hardware from booting up after deep sleep

#elif defined (ESP32)

	esp_sleep_enable_timer_wakeup(uint64_t(1000) * deepSleepTimeMs);
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

	return (resetInfo->reason == REASON_DEEP_SLEEP_AWAKE);

#elif defined (ESP32)

	esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

	return ((wakeup_reason == ESP_SLEEP_WAKEUP_EXT0 ) ||
			(wakeup_reason == ESP_SLEEP_WAKEUP_EXT1 ) ||
			(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER ) ||
			(wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD ) ||
			(wakeup_reason == ESP_SLEEP_WAKEUP_ULP ));

#endif
}

//========================================================================================================================
// ### MODEM-SLEEP ###
//
// Modem-sleep is the default mode for the ESP8266. However, it's only enabled when you're connected to an access point.
// While in Modem-sleep, the ESP8266 will disable the modem (WiFi) as much as possible. It turns off the modem between DTIM Beacon intervals.
// This interval is set by your router.
//
// Wroom-02 :
// Modem-sleep is used when such applications as PWM or I2S require the CPU to be working. In cases
// where Wi-Fi connectivity is maintained and data transmission is not required, the Wi-Fi Modem circuit can
// be shut down to save power, according to 802.11 standards (such as U-APSD). For example, in DTIM3,
// when ESP8266EX sleeps for 300 ms and wakes up for 3 ms to receive Beacon packages from AP, the
// overall average current consumption is about 15 mA.
//
// ### LIGHT-SLEEP ###
//
// Light-sleep performs the same function as Modem-sleep, but also turns off the system clock and suspends the CPU. The CPU isn't off; it's just idling.
// During Light-Sleep, the CPU may be suspended in applications like Wi-Fi switch. Without data transmission,
// the Wi-Fi Modem circuit can be turned off and CPU suspended to save power according to the 802.11 standard (U-APSD).
//
// Wroom-02 :
// Light-sleep is used for applications whose CPU may be suspended, such as Wi-Fi switch. In cases
// where Wi-Fi connectivity is maintained and data transmission is not required, the Wi-Fi Modem circuit and
// CPU can be shut down to save power, according to 802.11 standards (such as U-APSD). For example, in
// DTIM3, when ESP8266EX sleeps for 300 ms and wakes up for 3 ms to receive Beacon packages from
// AP, the overall average current consumption is about 0.9 mA.
//
// ### DEEP-SLEEP ###
//
// Everything is off but the Real Time Clock (RTC), which is how the computer keeps time. Since everything is off, this is the most power efficient option.
// When the chip goes out of deep sleep mode, it starts again at the start of the setup() function
// mode is one of WAKE_RF_DEFAULT, WAKE_RFCAL, WAKE_NO_RFCAL, WAKE_RF_DISABLED
// GPIO16 needs to be tied to RST pin to wake from deepSleep (On the NodeMCU, GPIO 16 is represented as D0)
// https://www.losant.com/blog/making-the-esp8266-low-powered-with-deep-sleep
//
// Wroom-02 :
// Deep-sleep is for applications that do not require Wi-Fi connectivity but only transmit data over long time
// lags, e.g., a temperature sensor that measures temperature every 100s. For example, when ESP8266EX
// sleeps for 300s then wakes up to connect to AP (taking about 0.3 ~ 1s), the overall average current
// consumption is far less than 1 mA. The current consumption of 20 μA was obtained at the voltage of 2.5V
// GPIO16; used for Deep-sleep wake-up when connected to RST pin.
//========================================================================================================================
void EspBoard :: enablePowerSavingMode (bool enable) {

	// *** Modem sleep & light sleep are just effective in station only mode ***
	if (WiFiHelper::isStationModeActive()) {

#ifdef ESP8266

		// Auto Light-sleep or Default mode
		wifi_set_sleep_type(enable ? LIGHT_SLEEP_T : MODEM_SLEEP_T);	// Just enable the function and let the system decide when it can sleep

		// WARNING !!
		// During Light-sleep, the CPU is suspended and will not respond to the signals and interrupts
		// from the peripheral hardware interfaces. Therefore, ESP8266 needs to be woken up via external GPIO.
		// The waking process is less than 3 ms.
		// The GPIO wake-up function can only be enabled by level triggers. The interface is as follows.
		// void gpio_pin_wakeup_enable(uint32 i, GPIO_INT_TYPE intr_state);

#elif defined (ESP32)

		// https://mischianti.org/esp32-practical-power-saving-manage-wifi-and-cpu-1/

		// WIFI_PS_NONE: disable modem sleep entirely;
		// WIFI_PS_MIN_MODEM: enable Modem-sleep minimum power save mode;
		// WIFI_PS_MAX_MODEM: to enable Modem-sleep maximum power save mode.

		// WiFi.setSleep(false); // the power consumption changes dramaticalyl, we have set WIFI_PS_NONE
		// WiFi.setSleep(true); // defult parameter, so we set WIFI_PS_MIN_MODEM

		// Maximum modem power saving. In this mode, interval to receive beacons is determined by the listen_interval
		// parameter in wifi_sta_config_t
		// Minimum modem power saving. In this mode, station wakes up to receive beacon every DTIM period
		esp_wifi_set_ps (enable ? WIFI_PS_MAX_MODEM : WIFI_PS_MIN_MODEM);

		// Another critical feature to reduce power consumption is cutting the CPU frequencies
		// if code using wifi or bt/ble, 80MHz is minimum you can go
		setCpuFrequencyMhz(/* The only value admitted : 240, 160, 80, 40, 20, 10 */ enable ? 80 : 240);
	}
	else if (!WiFiHelper::isWifiAvailable()) {
		// if code using wifi or bt/ble, 80MHz is minimum you can go
		setCpuFrequencyMhz(/* The only value admitted : 240, 160, 80, 40, 20, 10 */ enable ? 10 : 240);
#endif
	}
}

//========================================================================================================================
// Very risky to disable hardware watchdog!
//========================================================================================================================
void EspBoard :: enableHardwareWatchdog (bool enable) {
#ifdef ESP8266
	// ESP8266 and ESP32 have 2 watchdogs, one implemented in hardware and another in software
	if (enable) {
		*((volatile uint32_t*) 0x60000900) |= 1; // Hardware WDT ON
	}
	else {
		*((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
	}
#elif defined (ESP32)

	// @todo

#endif
}

//========================================================================================================================
// Very risky to disable software watchdog!
//========================================================================================================================
void EspBoard :: enableSoftwareWatchdog (bool enable) {
#ifdef ESP8266
	if (enable) {
		ESP.wdtEnable(5000);
	}
	else {
		ESP.wdtDisable();	// disable the software watchdog
	}
#elif defined (ESP32)
	// This will set the watchdog timeout to 5s and enable the controller reset if it’s triggered (reboot) or
	// This will set the watchdog timeout to 30s and disable the controller reset if it’s triggered
	esp_task_wdt_config_t wdt_config = {(uint32_t)(enable ? 5000 : 30000), enable};
	if (esp_task_wdt_init(&wdt_config) == ESP_OK) {
		Logln("Watchdog controller reset " << (enable ? "enabled" : "disabled") << " successfully !");
		// Add the current task to the Watchdog
		esp_task_wdt_add(NULL); // NULL = tâche actuelle
	}
	else {
		Logln("Error during Watchdog initialization!");
	}
#endif
}

}