//************************************************************************************************************************
// RemoteDebug.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>

#include "EspBoard.h"
#include "Logger.h"



SINGLETON_IMPL (Logger)


//========================================================================================================================
// Show time in millis
//========================================================================================================================
void Logger :: showTime (bool show) {
	_showTime = show;
}

//========================================================================================================================
// Show profiler - time in millis between messages of debug
//========================================================================================================================
void Logger :: showProfiler (bool show) {
	_showProfiler = show;
}

//========================================================================================================================
// Show colors
//========================================================================================================================
void Logger :: showColors (bool show) {
	_showColors = show;
}

//========================================================================================================================
// Show the name of this Esp
//========================================================================================================================
void Logger :: showChipName (bool show) {
	_showChipName = show;
}

//========================================================================================================================
// Print
//========================================================================================================================
size_t Logger :: write (uint8_t character) {

	static uint32_t lastTime = millis();
	static uint32_t elapsed = 0;

	// start a new line
	if ((_lineToPrint.isEmpty())&&(_showTime||_showProfiler||_showChipName)) {

		_lineToPrint = F("[");

		// Show time in millis
		if (_showTime) {
			_lineToPrint.concat (F("t:"));
			_lineToPrint.concat (millis());
			_lineToPrint.concat (F("ms"));
		}

		// Show profiler (time between messages)
		if (_showProfiler) {

			elapsed = (millis() - lastTime);
			bool resetColors = false;

			if (_showTime) _lineToPrint.concat (F(" "));

			if (_showColors) {

				if (elapsed < 250) {
					; // not color this
				}
				else if (elapsed < 1000) {
					_lineToPrint.concat (COLOR_BACKGROUND_CYAN);
					resetColors = true;
				}
				else if (elapsed < 3000) {
					_lineToPrint.concat (COLOR_BACKGROUND_YELLOW);
					resetColors = true;
				}
				else if (elapsed < 3000) {
					_lineToPrint.concat (COLOR_BACKGROUND_MAGENTA);
					resetColors = true;
				}
				else {
					_lineToPrint.concat (COLOR_BACKGROUND_RED);
					resetColors = true;
				}
			}

			_lineToPrint.concat (F("p:"));
			_lineToPrint.concat (formatNumber(elapsed, 4));
			_lineToPrint.concat (F("ms"));

			if (resetColors) {
				_lineToPrint.concat(COLOR_RESET);
			}

			lastTime = millis();
		}

		if (_showChipName) {
			if (_lineToPrint.length() > 1) _lineToPrint.concat (F(" "));
			_lineToPrint.concat(EspBoard::getDeviceName ());
		}

		_lineToPrint.concat (F("] "));
	}

	return LinePrinter :: write (character);
}

//========================================================================================================================
// Format numbers
//========================================================================================================================
String Logger::formatNumber(uint32_t value, uint8_t size, char insert) {

	// Putting zeroes in left
	String ret = "";

	for (uint8_t i=1; i<=size; i++) {
		uint32_t max = pow(10, i);
		if (value < max) {
			for (uint8_t j=(size - i); j>0; j--) {
				ret.concat(insert);
			}
			break;
		}
	}

	ret.concat(value);
	return ret;
}
