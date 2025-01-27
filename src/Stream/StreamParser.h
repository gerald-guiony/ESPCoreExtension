//************************************************************************************************************************
// StreamParser.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Stream.h>

namespace corex {
//------------------------------------------------------------------------------
//
class StreamParser
{
public:

	static bool checkNextStrInStream	(Stream & stream, const char * str);
	static uint8_t hexstr2Int 			(Stream & stream);

	virtual bool parse					(Stream & stream, Print & printer) = 0;
};
}













