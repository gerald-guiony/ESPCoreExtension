//************************************************************************************************************************
// LoggerCommandParser.h
// Version 2.0 December, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Print.h>

#include "Tools/Signal.h"

//------------------------------------------------------------------------------
//
class LoggerCommandParser
{
public:
	Signal <>								notifyCloseCurrentSessionResquested;
	Signal <>								notifyCloseAllSessionResquested;

public:
	LoggerCommandParser						() {}

	virtual bool parse						(char byteRcv, Print & printer);
};
