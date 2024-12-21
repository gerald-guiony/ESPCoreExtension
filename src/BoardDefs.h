//************************************************************************************************************************
// BoardDefs.h
// Version 1.0 October, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include "Global.h"



typedef struct sPin         	// used for input pins (pins for digitalRead)
{
	const uint8_t pin;         	// the GPIO
	const char *label;         	// a speaking name for the pin
}
Pin;

typedef struct sPWMPin			// used for PWM pins (pins for analogWrite)
{
	const uint8_t pin;         	// the GPIO
	const char *label;         	// a speaking name for the pin
	uint16_t value;            	// the current PWM level
}
PWMPin;

typedef struct sOutputPin   	// used for output pins (pins for digitalWrite mainly)
{
	const uint8_t pin;         	// the GPIO
	const char *label;         	// a speaking name for the pin
	const uint8_t ON;          	// level for ON (for active high set to HIGH)
}
OutputPin;


/*
#if ARDUINO_VERSION <= 106
#	warning Warning Arduino version <= 106 !!
#endif
*/

//------------------------------------------------------------------------------
// Set boards
//
#if defined(ARDUINO_ESP8266_GENERIC) || defined(ARDUINO_ESP8266_ESP01)
#	define ARDUINO_ESP8266_WIO_NODE
#endif


//------------------------------------------------------------------------------
// Boards definitions
//
// https://aisler.net/julienrat/wemostowio/wio-node-v1-0

#ifdef ARDUINO_ESP8266_WIO_NODE

//#	warning -- WIO_NODE defined --

#	define PORT0A		1
#	define PORT0B		3
#	define PORT1A		4
#	define PORT1B		5
#	define PORT_POWER	15											// (commun avec RED_LED)

#	define FUNC_BTN		0
#	define BLUE_LED		2											// D4 The BLUE led is attached to GPIO2 which is also the TX pin of UART1. When downloading firmware, the UART1 dumps the data transmitting on UART0 by instinct. So the BLUE led will blink while downloading firmware. After startup the GPIO2 will be configured as a GPIO not TX of UART1.
#	define RED_LED		PORT_POWER									// D5 The RED led is another status led which indicates the power status of Grove modules.
//#	define GREEN_LED	??											// D3 STAT : if no battery => led flash, if charging => led on, if charging ok => led off :) la LED verte ne peut pas être contrôlée car elle représente l'état de l'alimentation

#	define UART0_TX		PORT0A
#	define UART0_RX		PORT0B

#	define UART1_TX		2											// Debugging: UART1_TXD (GPIO2) can be used to print debugging information.

#	define I2C_SDA		PORT1A
#	define I2C_SCL		PORT1B

#	define BLINKLED		BLUE_LED
#	define USER_BTN		FUNC_BTN
#	define RST_BTN		PORT_POWER


#endif

// NodeMCU 1.0 12E
#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E

#	define D0			16
#	define D1			5											// I2C Bus SCL (clock)
#	define D2			4											// I2C Bus SDA (data)
#	define D3			0
#	define D4			2											// Same as "LEDBUILTIN", but inverted logic
#	define D5			14											// SPI Bus SCK (clock)
#	define D6			12											// SPI Bus MISO
#	define D7			13											// SPI Bus MOSI
#	define D8			15											// SPI Bus SS (CS)
#	define D9			3											// RX0 (Serial console)
#	define D10			1											// TX0 (Serial console)

#	define RST_BTN		D0
#	define FLASH_BTN	D3

#	define UART0_TX		D10											// UART Hardware Serial 1 (native) for debug
#	define UART0_RX		D9

#	define UART1_TX		D8											// UART Hardware Serial 2 (native)
#	define UART1_RX		D7

#	define I2C_SDA		D2
#	define I2C_SCL		D1

#	define LEDBUILTIN	D0

#	define BLINKLED		D4
#	define USER_BTN		FLASH_BTN

// HardwareSerial Serial(UART0);
// HardwareSerial Serial1(UART1);

#endif

#ifdef ARDUINO_WT32_ETH01

// assign pins to arrays:
// Pins, which should be readed
constexpr Pin inputPin[] {
  // the pin    a name
  //{1,    "IO01"}, 	// reserved for TX0 - used for first upload
  //{3,    "IO03"}, 	// reserved for RX0 - used for first upload
  {0,	"IO00"	},   	// needed to bring ESP into flash mode
  {39,  "I39"	},    	// input only
  {36,  "I36"	},    	// input only
  {15,  "IO15"	},
  {14,  "IO14"	},
  {12,  "IO12"	},
  {35,  "I35"	},    	// input only
};

// Pins, which can be switched on or off
constexpr OutputPin outputPin[] {
  // pin  a name  LOW active or HIGH active
  {32,	"IO32", 	LOW},
  {33,  "IO33", 	LOW},
  {17,  "IO17 LED", LOW},   // TX2; green LED (LOW active)
  {5,   "IO05 LED", LOW}    // RX2; green LED (LOW active)
};

// Pins, which can be dimmed
// the ESP32 can use a maximum of 16 pins for PWM
constexpr PWMPin pwmPin[] {
  //pin   a name  a PWM value
  {4,	"IO4",	1023},
  {2,	"IO2",	511	},
};

#	define FLASH_BTN	0
#	define BLINKLED		17
#	define USER_BTN		FLASH_BTN

#endif


//------------------------------------------------------------------------------
// Visual indicator
//

#if defined(ESP8266) || defined(ESP32)

#	define BLINKLED_ON()	({ if (getPinMode(BLINKLED) == OUTPUT) digitalWrite(BLINKLED, LOW);	})			// Inverted logic
#	define BLINKLED_OFF()	({ if (getPinMode(BLINKLED) == OUTPUT) digitalWrite(BLINKLED, HIGH);})

#else

//#	error Wrong chip defined - cannot continue: Please use an ESP8266

// Arduino Wifi
//#	define BLINKLED			13
//#	define BLINKLED_ON()	(PORTB |= B00100000)
//#	define BLINKLED_OFF()	(PORTB &= B11011111)

//#	define BLINKLED			-1
#	define BLINKLED_ON()	(Log(F(".")))
#	define BLINKLED_OFF()

#endif

#define BLINK()				do { BLINKLED_ON(); asyncDelayMillis (5); BLINKLED_OFF(); } while (0)
