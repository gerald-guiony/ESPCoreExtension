//************************************************************************************************************************
// Logger.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once


// The header file of the library might be able to see definitions from your sketch, the actual code (.c .cpp ) will not. 

// Define DEBUG for lots of lovely debug output :)
#define DEBUG
#define DEBUG_SERIAL_OUTPUT


//------------------------------------------------------------------------------
// Debug directives
//
#ifdef DEBUG
#	ifdef ESP8266
#		define LOGGER	I(Logger)
#	else
#		define LOGGER	Serial
#	endif
#	define Log(s)	(LOGGER << s)
#	define Logln(s) (LOGGER << s << LN)
#else
#	define Log(s)
#	define Logln(s)
#endif




#ifdef ESP8266

#include "Tools/Singleton.h"
#include "LinePrinter.h"


//------------------------------------------------------------------------------
// ANSI Colors
#define COLOR_RESET						"\x1B[0m"

#define COLOR_BLACK						"\x1B[0;30m"
#define COLOR_RED						"\x1B[0;31m"
#define COLOR_GREEN						"\x1B[0;32m"
#define COLOR_YELLOW					"\x1B[0;33m"
#define COLOR_BLUE						"\x1B[0;34m"
#define COLOR_MAGENTA					"\x1B[0;35m"
#define COLOR_CYAN						"\x1B[0;36m"
#define COLOR_WHITE						"\x1B[0;37m"

#define COLOR_DARK_BLACK				"\x1B[1;30m"
#define COLOR_DARK_RED					"\x1B[1;31m"
#define COLOR_DARK_GREEN				"\x1B[1;32m"
#define COLOR_DARK_YELLOW				"\x1B[1;33m"
#define COLOR_DARK_BLUE					"\x1B[1;34m"
#define COLOR_DARK_MAGENTA				"\x1B[1;35m"
#define COLOR_DARK_CYAN					"\x1B[1;36m"
#define COLOR_DARK_WHITE				"\x1B[1;37m"

#define COLOR_BACKGROUND_BLACK			"\x1B[40m"
#define COLOR_BACKGROUND_RED			"\x1B[41m"
#define COLOR_BACKGROUND_GREEN			"\x1B[42m"
#define COLOR_BACKGROUND_YELLOW			"\x1B[43m"
#define COLOR_BACKGROUND_BLUE			"\x1B[44m"
#define COLOR_BACKGROUND_MAGENTA		"\x1B[45m"
#define COLOR_BACKGROUND_CYAN			"\x1B[46m"
#define COLOR_BACKGROUND_WHITE			"\x1B[47m"



//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class Logger : public LinePrinter
{
	SINGLETON_CLASS(Logger)

private:

	bool _showTime 						= false;			// Show time in millis
	bool _showProfiler 					= false;			// Show time between messages
	bool _showColors 					= false;			// Show colors
	bool _showChipName					= false;			// Show the name of this Esp

	String formatNumber					(uint32_t value, uint8_t size, char insert='0');

public:

	void showTime						(bool show);
	void showProfiler					(bool show);
	void showColors						(bool show);
	void showChipName					(bool show);
	
	// Print
	virtual size_t write				(uint8_t) override;
};


#endif
