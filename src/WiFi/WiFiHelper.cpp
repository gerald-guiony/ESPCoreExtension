//************************************************************************************************************************
// WiFiHelper.cpp
// Version 1.0 December, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#ifdef ESP8266
#	include <ESP8266WiFi.h>							// https://github.com/esp8266/Arduino
#elif defined(ESP32)
#	include <WiFi.h>
#endif



#include "EspBoard.h"
#include "Print/Logger.h"

#include "WiFiHelper.h"

//========================================================================================================================
//
//========================================================================================================================
bool WiFiHelper :: isWifiAvailable () {
	// Access Point mode (server wifi) ou Station mode (client wifi)
	return (isAccessPointMode () || (WiFi.status() == WL_CONNECTED) /* assigned when connected to a WiFi network */);
}

//========================================================================================================================
//
//========================================================================================================================
bool WiFiHelper :: isAccessPointMode () {
	return (WiFi.getMode() == WIFI_AP);
}

//========================================================================================================================
//
//========================================================================================================================
IPAddress WiFiHelper :: getIpAddress () {
	return (isAccessPointMode () ? WiFi.softAPIP() : WiFi.localIP());
}

//========================================================================================================================
//
//========================================================================================================================
String WiFiHelper :: getMacAddress () {
	return (isAccessPointMode () ? WiFi.softAPmacAddress() : WiFi.macAddress());
}

//========================================================================================================================
// Connect device to wifi in station mode
// https://github.com/esp8266/Arduino/blob/4897e0006b5b0123a2fa31f67b14a3fff65ce561/doc/esp8266wifi/station-class.md
//========================================================================================================================
bool WiFiHelper :: connectToWiFi (uint16_t delayToConnect) {

	Logln(F("Connecting to wifi network.."));

	WiFiOn ();

	if (WiFi.status() == WL_CONNECTED) {
		Logln(F("Already connected!"));
		return true;
	}

	WiFi.mode(WIFI_STA);

	if (WiFi.SSID()) {
		Logln(F("Trying to connect to wifi ssid: ") << WiFi.SSID());
		WiFi.begin();									// Persistent mode
	}
	else {
//		WiFi.begin("freebox_ASUTRY", "*****22222");
	}

	unsigned long now = millis();
	while (millis() - now < delayToConnect) {			// Wait x ms to connect
		uint8_t status = WiFi.status();
		switch (status) {
			case WL_CONNECTED:
				Logln(F("Connected to wifi network !"));
				return true;

			case WL_CONNECT_FAILED:
				Logln(F("Can't connect to the wifi network, incorrect password !"));
				return false;

			case WL_NO_SSID_AVAIL:
				Logln(F("Can't connect to the wifi network, configured SSID cannot be reached !"));
				return false;

//			case WL_DISCONNECTED:
//				Logln(F("Can't connect to the wifi network, module is not configured in station mode !"));
//				return false;
		}
		delay (500);
	}

	Logln(F("Timeout, can't connect to the wifi network !"));
	return false;
}

//========================================================================================================================
// Wifi access point mode
//========================================================================================================================
void WiFiHelper :: startWiFiAccessPoint () {

	Logln(F("Starting access point.."));

	String AP_Name = EspBoard::getDeviceName();

	Logln(F("Configuring wifi access point : ") << AP_Name);

	WiFiOn ();

	WiFi.mode (WIFI_AP);
	WiFi.softAP (AP_Name.c_str());						//  to set up an open network with no WPA2-PSK password
}

//========================================================================================================================
//
//========================================================================================================================
void WiFiHelper :: resetWiFiHardware () {

#ifdef ARDUINO_ESP8266_WIO_NODE
	// RAZ du Wifi
	WiFi.forceSleepBegin();
	delay (200);										// Laisse du temps pour reinitialiser le hardware wifi apres un reset par le bouton power sur le Wio Node..
	WiFi.forceSleepWake();
#endif

}

//========================================================================================================================
//
//========================================================================================================================
void WiFiHelper :: disconnectAll () {

	if (!isWifiAvailable()) return;

	// WARNING !!!
	// Please note that functions WiFi.disconnect or WiFi.softAPdisconnect reset currently used SSID / password.
	// If persistent is set to false, then using these functions will not affect SSID / password stored in flash.

	WiFi.persistent(false);

	WiFi.softAPdisconnect(true);					// Delete the AP config complete from the espressif config memory
	WiFi.disconnect(true);							// Disconnects the station from an access point

	WiFi.persistent(true);

	delay(1);
}

//========================================================================================================================
// https://www.bakke.online/index.php/2017/05/21/reducing-wifi-power-consumption-on-esp8266-part-2/
//========================================================================================================================
void WiFiHelper :: WiFiOn () {

#ifdef ESP8266
	WiFi.forceSleepWake();
#endif

	WiFi.disconnect(false);  						// Reconnect the network

	delay(1);
}

//========================================================================================================================
// https://www.bakke.online/index.php/2017/05/21/reducing-wifi-power-consumption-on-esp8266-part-2/
//========================================================================================================================
void WiFiHelper :: WiFiOff () {

	disconnectAll();
	WiFi.mode(WIFI_OFF);

#ifdef ESP8266
	WiFi.forceSleepBegin();							// Switch off the radio, disable WiFi for 0xFFFFFFFF=4294967295 (32 bits) us which is about 71 minutes
#endif

	delay(1); 										// Give OS control to wake up wifi. yield() works also

	// WAKE_RF_DISABLED to keep the WiFi radio disabled when we wake up
//	ESP.deepSleep( SLEEPTIME, WAKE_RF_DISABLED );
}