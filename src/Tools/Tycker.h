//***********************************************************************************************
// Tycker.h
// Version 1.0 April, 2018
// Author Gerald Guiony
//***********************************************************************************************

#pragma once

#include <Ticker.h>

#include <stdbool.h>
#include <functional>
#include <Schedule.h>

//-----------------------------------------------------------------------------------------------
// Asynchronous callback, such as for the Ticker or ESPAsync* libs, have looser restrictions than
// ISRs, but some restrictions still apply. It is not possible to execute delay() or yield() from
// an asynchronous callback. Timing is not as tight as an ISR, but it should remain below a few
// milliseconds.
// The requirements of having all interrupt routines resident in ram does not apply to the calling
// chain for timers.
//
class Tycker : public Ticker
{
public:
	Tycker	() : Ticker () {};
	~Tycker	()	{ detach (); };

	void detach () {
		Ticker::detach ();
		_callback_function = nullptr;
	}

	typedef std::function<void(void)> callback_function_t;

	void attach_scheduled(float seconds, callback_function_t callback)
	{
		attach(seconds,std::bind(schedule_function, callback));
	}

	void attach(float seconds, callback_function_t callback)
	{
		_callback_function = callback;
		Ticker::attach(seconds, _static_callback, (void*)this);
	}

	void attach_ms_scheduled(uint32_t milliseconds, callback_function_t callback)
	{
		attach_ms(milliseconds, std::bind(schedule_function, callback));
	}

	void attach_ms(uint32_t milliseconds, callback_function_t callback)
	{
		_callback_function = callback;
		Ticker::attach_ms(milliseconds, _static_callback, (void*)this);
	}

	void once_scheduled(float seconds, callback_function_t callback)
	{
		once(seconds, std::bind(schedule_function, callback));
	}

	void once(float seconds, callback_function_t callback)
	{
		_callback_function = callback;
		Ticker::once(seconds, _static_callback, (void*)this);
	}

	void once_ms_scheduled(uint32_t milliseconds, callback_function_t callback)
	{
		once_ms(milliseconds, std::bind(schedule_function, callback));
	}

	void once_ms(uint32_t milliseconds, callback_function_t callback)
	{
		_callback_function = callback;
		Ticker::once_ms(milliseconds, _static_callback, (void*)this);
	}

	bool active()
	{
		return (bool)_timer;
	}

	void setCallbackFunction 	(callback_function_t callback)	{ _callback_function = callback;					}

	void startOnce 				(float seconds)					{ once (seconds, _callback_function);				}
	void startOnce_ms 			(uint32_t milliseconds)			{ once_ms (milliseconds, _callback_function);		}

	void startAttach			(float seconds)					{ attach (seconds, _callback_function);				}
	void startAttach_ms 		(uint32_t milliseconds)			{ attach_ms (milliseconds, _callback_function);		}

protected:

	static void _static_callback (void* arg)
	{
		Tycker* _this = (Tycker*)arg;
		if (_this == nullptr)
		{
			return;
		}
		if (_this->_callback_function)
		{
			_this->_callback_function();
		}
	}


protected:

	callback_function_t _callback_function = nullptr;
};
