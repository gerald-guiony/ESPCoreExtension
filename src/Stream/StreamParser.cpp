//************************************************************************************************************************
// StreamParser.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include "Print/Logger.h"

#include "StreamParser.h"

namespace corex {

//========================================================================================================================
//
//========================================================================================================================
bool StreamParser :: checkNextStrInStream (Stream & stream, const char * str)
{
	size_t len = strlen (str);

//	char buffer [len+1];
//	memset (buffer, 0, len+1);
//	stream.readBytes (buffer, len);
//	return (strcmp (buffer, str) == 0);

	for (int i=0; i<len; i++)
	{
		char c = stream.read();
		if (c != str [i])
		{
//			if ((c != '\n') && (c != '\r')) {
				Logln (c);
//			}
			return false;
		}
	}

	return true;
}


//========================================================================================================================
//
//========================================================================================================================
uint8_t StreamParser :: hexstr2Int (Stream & stream) {

	char hexValue [2];

	stream.readBytes (hexValue, 2);

	uint8_t tens = (hexValue[0] <= '9') ? hexValue[0] - '0' : hexValue[0] - '7';
	uint8_t ones = (hexValue[1] <= '9') ? hexValue[1] - '0' : hexValue[1] - '7';

	return (16 * tens) + ones;
}

}