//************************************************************************************************************************
// StreamCmdParser.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once


#include "StreamParser.h"


#define CMD_START					">> "
#define RESP_START					"<< "

#define MSG_TAG_BEGIN				"["
#define MSG_TAG_END					"]"
#define MSG_SEPARATOR_CMD			","
#define MSG_SEPARATOR_CMD_PARAM		"/"
#define MSG_SEPARATOR_RESP_PARAM	":"
#define MSG_SEPARATOR_PARAM			"|"

#define MSG_ACK						" OK"
#define MSG_NACK					" KO"
#define MSG_ERROR					" ERROR"

#define PRINT_CMD(id)				F(CMD_START) << F(MSG_TAG_BEGIN) << id << F(MSG_TAG_END)
#define PRINT_CMD_P(id,p)			F(CMD_START) << F(MSG_TAG_BEGIN) << id << F(MSG_SEPARATOR_CMD_PARAM) << p << F(MSG_TAG_END)
#define PRINT_RESP(id,p)			F(RESP_START) << F(MSG_TAG_BEGIN) << id << F(MSG_SEPARATOR_RESP_PARAM) << p << F(MSG_TAG_END)

#define PRINT_ACK(id)				PRINT_RESP(id,F(MSG_ACK))
#define PRINT_NACK(id)				PRINT_RESP(id,F(MSG_NACK))
#define PRINT_ERROR(id)				PRINT_RESP(id,F(MSG_ERROR))
#define PRINT_PARSE_FAILS(id)		F("ERROR: Invalid message or format") << LN << PRINT_ERROR(id)


namespace corex {

//------------------------------------------------------------------------------
//
class StreamCmdParser : public StreamParser
{
protected:

	bool checkCmdBegin				(Stream & stream);
	bool checkCmdEnd				(Stream & stream);
	bool checkSeparatorCmd			(Stream & stream);
	bool checkSeparatorCmdParam		(Stream & stream);
	bool checkSeparatorParam		(Stream & stream);
	int getCmdId					(Stream & stream);
	String getCmdParam				(Stream & stream);
};

//------------------------------------------------------------------------------
//
class StreamRespParser : public StreamParser
{
protected:

	bool checkRespBegin				(Stream & stream);
	bool checkRespEnd				(Stream & stream);
	bool checkSeparatorRespParam	(Stream & stream);
	bool checkSeparatorParam		(Stream & stream);
	int getRespId					(Stream & stream);
	String getRespParam				(Stream & stream);
};

}











