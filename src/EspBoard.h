//************************************************************************************************************************
// EspBoard.h
// Version 1.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Stream.h>
#include <StreamString.h>

#if !(defined (ESP8266) || defined (ESP32))
#	error Platform not supported
#endif

//------------------------------------------------------------------------------
// Static Class
//------------------------------------------------------------------------------
class EspBoard final
{
public:

	~EspBoard() = delete;	// you can not create an instance of such a class

	static void init 								(bool enableDebugSerial = false);
	static void reboot								();

	static const String getDeviceMemoryStats		();
	static uint32_t getDeviceId 					();
	static const String getDeviceName				();
	static const String getTimeElapsedSinceBoot		();
	static const String getResetReason				(int cpuNo = 0);

	static void setPortPower						(bool on);

	static void asyncDelayMillis 					(unsigned int ms);

	static int getPinMode 							(uint8_t pin);

	// Visual indicator
	static void blinkOn								();
	static void blinkOff							();
	static void blink								();
	static void blinks								(int nbBlink);

	static void enterDeepSleep						(unsigned long deepSleepDurationMs);
	static bool isWakeUpFromDeepSleep				();

	// Very risky !
	static void disableHardwareWatchdog				();
	static void enableHardwareWatchdog				();
};


