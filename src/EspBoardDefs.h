//************************************************************************************************************************
// EspBoardDefs.h
// Version 1.0 October, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

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
#	define PORT_POWER	15							// (commun avec RED_LED)

#	define FUNC_BTN		0
#	define BLUE_LED		2							// D4 The BLUE led is attached to GPIO2 which is also the TX pin of UART1. When downloading
													// firmware, the UART1 dumps the data transmitting on UART0 by instinct. So the BLUE led will
													// blink while downloading firmware. After startup the GPIO2 will be configured as a GPIO not TX of UART1.
#	define RED_LED		PORT_POWER					// D5 The RED led is another status led which indicates the power status of Grove modules.
//#	define GREEN_LED	??							// D3 STAT : if no battery => led flash, if charging => led on, if charging ok => led off :) la LED verte
													// ne peut pas être contrôlée car elle représente l'état de l'alimentation

#	define UART0_TX		PORT0A
#	define UART0_RX		PORT0B

#	define UART1_TX		2							// Debugging: UART1_TXD (GPIO2) can be used to print debugging information.

#	define I2C_SDA		PORT1A
#	define I2C_SCL		PORT1B

#	define BLINKLED		BLUE_LED
#	define USER_BTN		FUNC_BTN
#	define RST_BTN		PORT_POWER


#endif

// NodeMCU 1.0 12E
#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E

#	define D0			16
#	define D1			5							// I2C Bus SCL (clock)
#	define D2			4							// I2C Bus SDA (data)
#	define D3			0
#	define D4			2 							// Same as "LEDBUILTIN", but inverted logic
#	define D5			14							// SPI Bus SCK (clock)
#	define D6			12							// SPI Bus MISO
#	define D7			13							// SPI Bus MOSI
#	define D8			15							// SPI Bus SS (CS)
#	define D9			3 							// RX0 (Serial console)
#	define D10			1 							// TX0 (Serial console)

#	define RST_BTN		D0
#	define FLASH_BTN	D3

#	define UART0_TX		D10							// UART Hardware Serial 1 (native) for debug
#	define UART0_RX		D9

#	define UART1_TX		D8							// UART Hardware Serial 2 (native)
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

//------------------------------------------------------------------------------
// How to program the WT32-ETH01 by USB (before doing it via OTA)
//------------------------------------------------------------------------------
//
// The board can be programmed when you connect a USB-TTL adapter to the respective
// pins. Connect the RX0/TX0/GND to the TX/RX/GND of your USB-TTL device. I have
// set the level to 3V3 but I'm sourcing the 5V from the USB-Adapter.
//
// To load code, the ESP32 needs the IO0 (BOOT) pin grounded while toggling EN
// To manually reset a development board, hold down the Boot button (GPIO0) and
// press the EN button
//
// To bring the WT32-EHT01 in program mode I follow this sequence:
//
// disconnect VCC
// connect EN with GND
// connect VCC
// disconnect EN from GND
//
// You should see a following in the serial monitor:
//
// rst:0x1 (POWERON_RESET),boot:0x3 (DOWNLOAD_BOOT(UART0/UART1/SDIO_REI_REO_V2))
// waiting for download
//------------------------------------------------------------------------------

// Input pins
//# define IO01	 		1 							// reserved for TX0 - used for first upload
//# define IO03	 		3 							// reserved for RX0 - used for first upload
#	define IO00	   		0 							// needed to bring ESP into flash mode
#	define I39			39							// input only
#	define I36			36							// input only
#	define IO15	   		15
#	define IO14	   		14
#	define IO12	   		12
#	define I35			35							// input only

// Ouput pins
// Pins, which can be switched on or off
#	define IO32	   		32
#	define IO33	   		33
#	define IO17_LED   	17							// TX2; green LED (LOW active)
#	define IO05_LED   	5				 			// RX2; green LED (LOW active)

// PWM pins
// Pins, which can be dimmed
// the ESP32 can use a maximum of 16 pins for PWM
#	define IO4			4				 			// current PWM level = 1023
#	define IO2			2				 			// current PWM level = 511

#	define FLASH_BTN	IO00
#	define BLINKLED		IO05_LED
#	define USER_BTN		I36

#endif

