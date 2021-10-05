//************************************************************************************************************************
// WiFiHelper.h
// Version 1.0 December, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <IPAddress.h>

#define STA_CONNECTION_DELAY				20000


//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class WiFiHelper
{

public:

	static bool isWifiAvailable				();
	static bool isAccessPointMode			();
	static IPAddress getIpAddress			();
	static String getMacAddress				();

	static bool connectToWiFi				(uint16_t delayToConnect = STA_CONNECTION_DELAY);
	static void startWiFiAccessPoint		();

	static void resetWiFiHardware			();
	static void disconnectAll				();

	static void WiFiOn						();
	static void WiFiOff						();

};