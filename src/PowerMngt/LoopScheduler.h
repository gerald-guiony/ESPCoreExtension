//************************************************************************************************************************
// LoopScheduler.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

// http://bbs.espressif.com/viewtopic.php?f=7&t=171&p=617
// http://jeelabs.org/book/1526f/
// http://blog.mister-bidouilles.fr/surveillez-la-consommation-de-vos-montages/


#pragma once

#include <list>

#include "Tools/Singleton.h"
#include "Tools/Delegate.h"

#include "Looper.h"


// ### MODEM-SLEEP ###

// Modem-sleep is the default mode for the ESP8266. However, it's only enabled when you're connected to an access point.
// While in Modem-sleep, the ESP8266 will disable the modem (WiFi) as much as possible. It turns off the modem between DTIM Beacon intervals. 
// This interval is set by your router.

// Wroom-02 :
// Modem-sleep is used when such applications as PWM or I2S require the CPU to be working. In cases
// where Wi-Fi connectivity is maintained and data transmission is not required, the Wi-Fi Modem circuit can
// be shut down to save power, according to 802.11 standards (such as U-APSD). For example, in DTIM3,
// when ESP8266EX sleeps for 300 ms and wakes up for 3 ms to receive Beacon packages from AP, the
// overall average current consumption is about 15 mA.

// ### LIGHT-SLEEP ###

// Light-sleep performs the same function as Modem-sleep, but also turns off the system clock and suspends the CPU. The CPU isn't off; it's just idling.
// During Light-Sleep, the CPU may be suspended in applications like Wi-Fi switch. Without data transmission,
// the Wi-Fi Modem circuit can be turned off and CPU suspended to save power according to the 802.11 standard (U-APSD).

// Wroom-02 :
// Light-sleep is used for applications whose CPU may be suspended, such as Wi-Fi switch. In cases
// where Wi-Fi connectivity is maintained and data transmission is not required, the Wi-Fi Modem circuit and
// CPU can be shut down to save power, according to 802.11 standards (such as U-APSD). For example, in
// DTIM3, when ESP8266EX sleeps for 300 ms and wakes up for 3 ms to receive Beacon packages from
// AP, the overall average current consumption is about 0.9 mA.

// ### DEEP-SLEEP ###

// Everything is off but the Real Time Clock (RTC), which is how the computer keeps time. Since everything is off, this is the most power efficient option. 
// When the chip goes out of deep sleep mode, it starts again at the start of the setup() function
// mode is one of WAKE_RF_DEFAULT, WAKE_RFCAL, WAKE_NO_RFCAL, WAKE_RF_DISABLED
// GPIO16 needs to be tied to RST pin to wake from deepSleep (On the NodeMCU, GPIO 16 is represented as D0)
// https://www.losant.com/blog/making-the-esp8266-low-powered-with-deep-sleep

// Wroom-02 :
// Deep-sleep is for applications that do not require Wi-Fi connectivity but only transmit data over long time
// lags, e.g., a temperature sensor that measures temperature every 100s. For example, when ESP8266EX
// sleeps for 300s then wakes up to connect to AP (taking about 0.3 ~ 1s), the overall average current
// consumption is far less than 1 mA. The current consumption of 20 μA was obtained at the voltage of 2.5V
// GPIO16; used for Deep-sleep wake-up when connected to RST pin.

enum class SleepMode {
	ModemSleep			= 1,
	AutoLightSleep		= 2,
	ForcedLightSleep	= 3,
	DeepSleep			= 4
};

#define SHORT_DELAY_BETWEEN_EXEC_MS					1000
#define LONG_DELAY_BETWEEN_EXEC_MS					10000

#define WAKEUP_DURATION_MS							60000

#define DEEP_SLEEP_DURATION_MS						3600000						// 1h en ms.. with µs, maximum value is 0xFFFFFFFF=4294967295 (32 bits) µs which is about 71 minutes

#define DEEPSLEEP_NAMEFILE							"/deepsleep.txt"


//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class LoopScheduler : public Looper
{
	SINGLETON_CLASS(LoopScheduler)

private:

	SleepMode _sleepMode						= SleepMode::ModemSleep;

	unsigned long _shortDelayBetweenExecMs		= SHORT_DELAY_BETWEEN_EXEC_MS;
	unsigned long _longDelayBetweenExecMs		= LONG_DELAY_BETWEEN_EXEC_MS;
	unsigned long _wakeUpDurationMs				= WAKEUP_DURATION_MS;

	unsigned long _deepSleepDurationMs 			= DEEP_SLEEP_DURATION_MS;
	std::function <bool()> _isOkToDeepSleepFn	= [] { return false; };
	

	volatile unsigned long _lastWakeUpTimeStamp	= 0;				// We need to declare a variable as volatile when it can be changed unexpectedly
																	// (as in an ISR), so the compiler doesn’t remove it due to optimizations
	unsigned long _lastExecTimeStamp			= 0;

	bool _wakeUpState							= false;
	bool _requestReboot							= false;

	std::list <Looper *> 						_loopers;
	std::list <Looper *> :: iterator 			_itLooper;

private:
	
	bool updateWakeUpState						();
	bool isOkToEnterDeepSleep					() const;

public:

	Delegate <bool> notifyWakeUpStateChanged;

public:
	
	void requestReboot							();
	void wakeUp		 							();

	void setShortDelayBetweenExecution			(unsigned long shortDelayBetweenExecMs)		{ _shortDelayBetweenExecMs	= shortDelayBetweenExecMs;	}
	void setLongDelayBetweenExecution			(unsigned long longDelayBetweenExecMs)		{ _longDelayBetweenExecMs	= longDelayBetweenExecMs;	}
	void setWakeUpDuration						(unsigned long wakeUpDurationMs)			{ _wakeUpDurationMs	= wakeUpDurationMs;					}

	void setDeepSleepDuration					(unsigned long deepSleepDurationMs)			{ _deepSleepDurationMs		= deepSleepDurationMs;		}
	void setEnterDeepSleepCond					(std::function <bool()> isOkToDeepSleepFn)	{ _isOkToDeepSleepFn		= isOkToDeepSleepFn;		}
	void enterDeepSleepWhenWifiOff				();
	void enterDeepSleep							() const;
	bool isWakeUpFromDeepSleep					() const;

	void setLoopers 							(std::list <Looper *> loopers);

	void setup									(std::list <Looper *> loopers, SleepMode sleepMode = SleepMode::ModemSleep);
	void loop									() override;
};













