//************************************************************************************************************************
// Global.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Stream.h>
#include <StreamString.h>

#ifdef ESP8266

void initSketch 				(bool enableDebugSerial = true);
void reboot						();

String getChipMemoryStats		();
String getChipName				();

// Very risky !
void disableHardwareWatchdog	();
void enableHardwareWatchdog		();

#endif

void asyncDelayMillis 			(unsigned int ms);
String timeElapsedSinceBoot		();
bool strToLong 					(const String & str, long & value);
void blinkStatus				(int nbBlink);
int getPinMode 					(uint8_t pin);




