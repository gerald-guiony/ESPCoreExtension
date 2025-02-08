//************************************************************************************************************************
// AsyncModule.h
// Version 1.0 Jan, 2025
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

namespace corex {

//------------------------------------------------------------------------------
//
class IAsyncModule
{
public:
    virtual ~IAsyncModule() = default;
};

//------------------------------------------------------------------------------
//
template <typename ...Args>
class AsyncModule : public IAsyncModule
{
public:
	virtual void setup (Args ...args) = 0;
};

}