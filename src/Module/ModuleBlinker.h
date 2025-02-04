//************************************************************************************************************************
// ModuleBlinker.h
// Version 1.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include "Module.h"
#include "EspBoard.h"
#include "Tools/Singleton.h"

namespace corex {

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class ModuleBlinker : public Module <>
{
	SINGLETON_INLINE(ModuleBlinker)

public :
	virtual void setup	() override {}
	virtual void loop	() override {
		EspBoard::blink();
	};
};

}