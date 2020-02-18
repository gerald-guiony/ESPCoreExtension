//************************************************************************************************************************
// StreamCmdParser.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include "Print/Logger.h"

#include "StreamCmdParser.h"


//========================================================================================================================
//
//========================================================================================================================
bool StreamCmdParser :: checkCmdBegin (Stream & stream) {

	if (!checkNextStrInStream (stream, CMD_START)) {
		return false;
	}

	// Format of the command [CommandId/Param1/Param2/...]
	if (!checkNextStrInStream (stream, MSG_TAG_BEGIN)) {
		return false;
	}

	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool StreamCmdParser :: checkCmdEnd (Stream & stream) {
	return (checkNextStrInStream (stream, MSG_TAG_END));
}

//========================================================================================================================
//
//========================================================================================================================
bool StreamCmdParser :: checkSeparatorCmd (Stream & stream) {
	return (checkNextStrInStream (stream, MSG_SEPARATOR_CMD));
}

//========================================================================================================================
//
//========================================================================================================================
bool StreamCmdParser :: checkSeparatorCmdParam (Stream & stream) {
	return (checkNextStrInStream (stream, MSG_SEPARATOR_CMD_PARAM));
}

//========================================================================================================================
//
//========================================================================================================================
bool StreamCmdParser :: checkSeparatorParam (Stream & stream) {
	return (checkNextStrInStream (stream, MSG_SEPARATOR_PARAM));
}

//========================================================================================================================
//
//========================================================================================================================
int StreamCmdParser :: getCmdId (Stream & stream) {
	// read the command Id
	int commandId = stream.parseInt();
	if ((commandId < 0) || (10 < commandId)) {
		Logln (F("commandId < 0 or > 10 ???"));
		return -1;
	}
	return commandId;
}

//========================================================================================================================
//
//========================================================================================================================
String StreamCmdParser :: getCmdParam (Stream & stream) {
	
	String ret;
	int p = stream.peek();
	
	while (p >= 0) {
		String checkStr;
		int i = 1;
		
		// Le prochain separateur est il MSG_SEPARATOR_PARAM, MSG_SEPARATOR_CMD ou MSG_TAG_END ?
		if (p == MSG_SEPARATOR_PARAM[0]) checkStr = MSG_SEPARATOR_PARAM;
		if (p == MSG_SEPARATOR_CMD[0]) checkStr = MSG_SEPARATOR_CMD;
		if (p == MSG_TAG_END[0]) checkStr = MSG_TAG_END;
		
		if (checkStr.length() > 0) {
			for (i = 1; i<checkStr.length(); i++) {
				p = stream.peek ();
				if (p != checkStr[i]) break;
			}
			if (i == checkStr.length()) return ret;
		}
		
		ret += (char)stream.read();
		p = stream.peek();
	}
	
	return ret;
}




//************************************************************************************************************************
//************************************************************************************************************************
//************************************************************************************************************************




//========================================================================================================================
//
//========================================================================================================================
bool StreamRespParser :: checkRespBegin (Stream & stream) {

	if (!checkNextStrInStream (stream, RESP_START)) {
		return false;
	}

	// Format of the command [CommandId/Param1/Param2/...]
	if (!checkNextStrInStream (stream, MSG_TAG_BEGIN)) {
		return false;
	}

	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool StreamRespParser :: checkRespEnd (Stream & stream) {
	return (checkNextStrInStream (stream, MSG_TAG_END));
}

//========================================================================================================================
//
//========================================================================================================================
bool StreamRespParser :: checkSeparatorRespParam (Stream & stream) {
	return (checkNextStrInStream (stream, MSG_SEPARATOR_RESP_PARAM));
}

//========================================================================================================================
//
//========================================================================================================================
bool StreamRespParser :: checkSeparatorParam (Stream & stream) {
	return (checkNextStrInStream (stream, MSG_SEPARATOR_PARAM));
}

//========================================================================================================================
//
//========================================================================================================================
int StreamRespParser :: getRespId (Stream & stream) {
	// read the command Id
	int respId = stream.parseInt();
	if ((respId < 0) || (10 < respId)) {
		Logln (F("respId < 0 or > 10 ???"));
		return -1;
	}
	return respId;
}

//========================================================================================================================
//
//========================================================================================================================
String StreamRespParser :: getRespParam (Stream & stream) {
	
	String ret;
	int p = stream.peek();
	
	while (p >= 0) {
		String checkStr;
		int i = 1;
		
		// Le prochain separateur est il MSG_SEPARATOR_PARAM ou MSG_TAG_END ?
		if (p == MSG_SEPARATOR_PARAM[0]) checkStr = MSG_SEPARATOR_PARAM;
		if (p == MSG_TAG_END[0]) checkStr = MSG_TAG_END;
		
		if (checkStr.length() > 0) {
			for (i = 1; i<checkStr.length(); i++) {
				p = stream.peek ();
				if (p != checkStr[i]) break;
			}
			if (i == checkStr.length()) return ret;
		}
		
		ret += (char)stream.read();
		p = stream.peek();
	}
	
	return ret;
}

