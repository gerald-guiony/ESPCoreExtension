//************************************************************************************************************************
// LinePrinter.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include "LinePrinter.h"


//========================================================================================================================
// Initialize
//========================================================================================================================
LinePrinter :: LinePrinter () {
	_lineToPrint.reserve (BUFFER_PRINT_LEN);
	flush ();
}

//========================================================================================================================
//
//========================================================================================================================
void LinePrinter :: flush () {
	// Empty the buffer
	_lineToPrint = "";
}

//========================================================================================================================
// Print
//========================================================================================================================
size_t LinePrinter :: write (uint8_t character) {

	size_t result = 0;

	// Print ?
	bool doPrint = false;

	// New line ?
	if ((character == ln) || (character == cr)) {
		character = ln;
		doPrint = true;
	}

	// Write to Buffer
	bool concatOk = _lineToPrint.concat ((char)character);

	if (_lineToPrint.length() >= BUFFER_PRINT_LEN - 2) {	// Limit of buffer (we need at least 2 bytes for LN)
		doPrint = true;
	}

	// Send the characters buffered by print.h
	if (doPrint && concatOk) {								// Print the buffer
		notifyRequestLineToPrint (_lineToPrint);
		result = _lineToPrint.length();
		flush ();
	}

	return result;
}

