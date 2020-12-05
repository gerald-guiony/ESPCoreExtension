//************************************************************************************************************************
// BasicRelay.h
// Version 1.0 August, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once


#include <Arduino.h>

#include "Global.h"



/**
 * GroveRelay Description example: 
 * The Grove-Relay module is a digital normally-open switch. Through it, you can control circuit of high voltage with low 
 * voltage, say 5V on the controller. There is an indicator LED on the board, which will light up when the controlled 
 * terminals get closed.
 * 
 * Operating Voltage		3.3V~5V
 * Operating Current		100mA
 * Max Switching Voltage	250VAC/30VDC
 * Max Switching Current	5A
 */
class BasicRelay 
{
private:
	uint8_t _pin;

public:

	BasicRelay		(uint8_t pin) : _pin(pin)	{	pinMode (_pin, OUTPUT);
													open ();						}

	void open		()							{ 	digitalWrite (_pin, LOW);
													// Power OFF the Grove sockets
													setModulesPower (false);		}

	void close		()							{ 	// Power ON the Grove sockets
													setModulesPower (true);
													digitalWrite (_pin, HIGH );		}
};
