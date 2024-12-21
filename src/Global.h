//************************************************************************************************************************
// Global.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Stream.h>
#include <StreamString.h>

#if defined (ESP8266) || defined (ESP32)

String getChipMemoryStats		();
uint32_t getChipId 				();

void setModulesPower			(bool on);

const String get_reset_reason	(int cpuNo = 0);

void asyncDelayMillis 			(unsigned int ms);

// Very risky !
void disableHardwareWatchdog	();
void enableHardwareWatchdog		();

#endif

void initSketch 				(bool enableDebugSerial = false);
String getChipName				();
void reboot						();

String timeElapsedSinceBoot		();
bool strToLong 					(const String & str, long & value);
void blinkStatus				(int nbBlink);
int getPinMode 					(uint8_t pin);




