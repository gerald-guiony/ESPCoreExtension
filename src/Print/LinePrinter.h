//************************************************************************************************************************
// LinePrinter.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Print.h>

#include "Tools/Signal.h"

//------------------------------------------------------------------------------
// Streaming template
//
template<class T> inline Print & operator <<(Print & printer, T arg) { printer.print(arg); return printer; }

#define _cr				'\r'
#define _ln				'\n'
#define LN				F("\n")


// Buffered print write to telnet -> length of buffer
#define BUFFER_PRINT_LEN				200


//------------------------------------------------------------------------------
//
class LinePrinter : public Print
{
protected:

	String _lineToPrint 				= "";				// Buffer of print write to telnet

public:

	Signal <const String &>				notifyRequestLineToPrint;

public:

	LinePrinter 						();

	virtual void flush					() override;

	// Print
	virtual size_t write				(uint8_t) override;
};



//========================================================================================================================
//
//========================================================================================================================
template <typename IntType>
String n2hexstr (IntType w, size_t hex_len = sizeof(IntType)<<1) {
    static const char* digits = "0123456789ABCDEF";
    String rc;
	rc.reserve (hex_len);
	for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
        rc += digits [ (w>>j) & 0x0F ];
    return rc;
}
