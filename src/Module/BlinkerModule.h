//************************************************************************************************************************
// BlinkerModule.h
// Version 1.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include "Module.h"
#include "EspBoard.h"
#include "Tools/Singleton.h"

namespace corex {

//------------------------------------------------------------------------------
//
class BlinkerModule : public Module <>
{
	SINGLETON_INLINE(BlinkerModule)

public :
	virtual void setup	() override {}
	virtual void loop	() override {
		EspBoard::blinks (2);
	};
};

}