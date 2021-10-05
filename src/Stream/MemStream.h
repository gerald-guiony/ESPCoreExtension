//************************************************************************************************************************
// MemStream.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <inttypes.h>
#include <Stream.h>



#define BUF_MAX_LEN			100


//------------------------------------------------------------------------------
//
class MemStream : public Stream
{
private:
	char					_buffer [BUF_MAX_LEN];

	bool					_buffer_overflow;
	uint16_t				_pos_read;
	uint16_t				_pos_write;

	Stream *				_fileStream = NULL;					// Stream when oveflow

public:
	// public methods
	MemStream				();
	MemStream				(Stream & stream);
	~MemStream				();

	//operator const uint8_t *() const { return _buffer; }
	//operator const char *() const { return (const char*)_buffer; }

	bool overflow			() { return _buffer_overflow; }

	size_t readFrom			(Stream & stream);
	size_t writeTo			(Stream & stream);

	virtual size_t write	(uint8_t byte) override;
	virtual size_t write	(const uint8_t *buf, size_t size) override;
	virtual int read		() override;
	virtual int peek		() override;
	virtual int available	() override;
	virtual void flush		() override;

	//using Print::write;

//	friend MemStream& operator << (MemStream & memStream, Stream & stream) {
//		memStream.readFrom(stream);
//		return memStream;
//	}

	friend MemStream& operator >> (MemStream & memStream, Stream & stream) {
		memStream.writeTo(stream);
		return memStream;
	}

};

