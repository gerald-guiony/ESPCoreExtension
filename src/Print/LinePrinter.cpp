//************************************************************************************************************************
// LinePrinter.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include "LinePrinter.h"


namespace corex {

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
	_lineToPrint = F("");
}

//========================================================================================================================
// Print
//========================================================================================================================
size_t LinePrinter :: write (uint8_t character) {

	// Print ?
	bool doPrint = false;

	// New line ?
	if ((character == _ln) || (character == _cr)) {
		character = _ln;
		doPrint = true;
	}

	// Write to Buffer
	_lineToPrint.concat ((char)character);

	if (_lineToPrint.length() >= BUFFER_PRINT_LEN - 2) {	// Limit of buffer (we need at least 2 bytes for LN)
		doPrint = true;
	}

	// Send the characters buffered by print.h
	if (doPrint) {							// Print the buffer
		notifyRequestLineToPrint (_lineToPrint);
		flush ();
	}

	return 1;  // Don't break the print of the buffer !
}

}