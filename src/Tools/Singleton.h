//***********************************************************************************************
// Singleton.h
// Version 1.0 April, 2019
// Author Gerald Guiony
//***********************************************************************************************

#pragma once

//-----------------------------------------------------------------------------------------------
// Simplified writing
//
#define I(x)					x::getInstance()


// ==============================================================================================
// Meyers Singleton
// The Meyers Singleton in C++11 is automatically  ***  THREAD SAFE ***
// guaranteed by the standard: Static variables with block scope
// ==============================================================================================

#define SINGLETON_CLASS(className)																\
public:																							\
	/*--------------------------------------------------------------------------------------*/	\
	/* Class used within library, in other library and also in main application 			*/	\
	/* => move the static Singleton instance into cpp										*/	\
	static className & getInstance ();															\
																								\
	/*--------------------------------------------------------------------------------------*/	\
	/* C++ 11																				*/	\
	/* =======																				*/	\
	/* Don't forget to declare these two. You want to make sure they are unacceptable 		*/	\
	/* otherwise you may accidentally get copies of your singleton appearing.				*/	\
	/* Note: deleted functions should generally be public as it results in better error		*/	\
	/* messages due to the compilers behavior to check accessibility before deleted status	*/	\
	className						(const className&) = delete;								\
	className& operator=			(const className&) = delete;								\
																								\
private:																						\
	/* private constructor and destructor													*/	\
	className						();															\
	virtual ~className				();


#define SINGLETON_INST(className)																\
	/*--------------------------------------------------------------------------------------*/	\
	/* Guaranteed to be instanciated only one time even in dll								*/	\
	className & className::getInstance()														\
	{																							\
		static className instance; /* Guaranteed to be destroyed, instantiated on first use */	\
		return instance;																		\
	}


#define SINGLETON_IMPL(className)																\
	className :: className			() {}														\
	className::~className			() {}														\
																								\
	SINGLETON_INST(className)



