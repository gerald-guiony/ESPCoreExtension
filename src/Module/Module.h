//************************************************************************************************************************
// Module.h
// Version 2.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

namespace corex {

//------------------------------------------------------------------------------
//
class IModule
{
public:
    virtual ~IModule() = default;
	virtual void loop () = 0;
};

//------------------------------------------------------------------------------
//
template <typename ...Args>
class Module  : public IModule
{
public:
	virtual void setup (Args ...args) = 0;
};

}