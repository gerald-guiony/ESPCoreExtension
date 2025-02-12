//************************************************************************************************************************
// WiFiHelper.h
// Version 1.0 December, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <IPAddress.h>

#define STA_CONNECTION_TIME				20000


namespace corex {

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class WiFiHelper
{

public:

	static bool isWifiAvailable				();
	static bool isAccessPointMode			();
	static bool isStationModeActive			();

	static IPAddress getIpAddress			();
	static String getMacAddress				();

	static bool connectToWiFi				(uint16_t delayToConnect = STA_CONNECTION_TIME);
	static void startWiFiAccessPoint		();

	static void resetWiFiHardware			();
	static void disconnectAll				();

	static void WiFiOn						();
	static void WiFiOff						();

};

}