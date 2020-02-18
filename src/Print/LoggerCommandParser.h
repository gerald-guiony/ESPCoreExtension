//************************************************************************************************************************
// LoggerCommandParser.h
// Version 2.0 December, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Print.h>

#include "Tools/Delegate.h"

//------------------------------------------------------------------------------
//
class LoggerCommandParser
{
public:
	Delegate <>								notifyCloseCurrentSessionResquested;
	Delegate <>								notifyCloseAllSessionResquested;

public:
	LoggerCommandParser						() {}

	virtual bool parse						(char byteRcv, Print & printer);
};
