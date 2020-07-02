//************************************************************************************************************************
// LoggerCommandParser.cpp
// Version 2.0 December, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>
#include <WCharacter.h>

#include "Global.h"
#include "PowerMngt/LoopScheduler.h"

#include "Logger.h"
#include "LoggerCommandParser.h"


#define PRINT_HELP																											 \
	F("-------------------------------------------------------------------------------------------------------")	<< LN << \
	F(" *** AVAILABLE DEBUG COMMANDS *** ")																			<< LN << \
	F("-------------------------------------------------------------------------------------------------------")	<< LN << \
	F("? or h -> display these help of commands")																	<< LN << \
	F("q -> quit (close current telnet session)")																	<< LN << \
	F("m -> display memory available")																				<< LN << \
	F("t -> show time (millis)")																					<< LN << \
	F("p -> show time between actual and last message (in millis)")													<< LN << \
	F("c -> show colors")																							<< LN << \
	F("r -> reset the ESP8266")																						<< LN << \
	F("-------------------------------------------------------------------------------------------------------")


//========================================================================================================================
//
//========================================================================================================================
bool LoggerCommandParser :: parse (char byteRcv, Print & printer) {
	
	if (byteRcv == -1			|| 
		isWhitespace (byteRcv)	||
		isSpace (byteRcv)		||
		!isAscii (byteRcv)		||
		!isPrintable (byteRcv))	{
		return false;
	}
	
	switch (byteRcv)
	{
	case '?':
	case 'h':
		printer << LN << PRINT_HELP << LN;
		break;

	case 'q':
		notifyCloseCurrentSessionResquested ();
		break;
			
	case 'm':
		printer << getChipMemoryStats ();
		break;

	case 't':
		I(Logger).showTime(true);
		break;

	case 'p':
		I(Logger).showProfiler(true);
		break;
	
	case 'c':
		I(Logger).showColors(true);
		break;

	case 'r':
		printer << F("=> Reboot") << LN;
		printer << F("Bye bye...") << LN;

		notifyCloseAllSessionResquested ();
		I(LoopScheduler).requestReboot ();
		break;
		
	default:
		printer << F("[") << byteRcv << F("]: unknow command") << LN;
		return false;
	}

	return true;
}

