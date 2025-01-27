//************************************************************************************************************************
// MemStream.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include "Print/Logger.h"
#include "Storage/FileStorage.h"

#include "MemStream.h"

namespace corex {

//========================================================================================================================
//
//========================================================================================================================
MemStream::MemStream () :
	_buffer_overflow(false), _pos_write(0), _pos_read(0)
{
	FileStorage::init ();
	memset(_buffer, 0, BUF_MAX_LEN * sizeof (_buffer[0]));
}

//========================================================================================================================
// Constructeur par copie
//========================================================================================================================
MemStream::MemStream (Stream & stream)
:_buffer_overflow(false), _pos_write(0), _pos_read(0)
{
	FileStorage::init ();
	flush ();
	readFrom (stream);
}

//========================================================================================================================
//
//========================================================================================================================
MemStream::~MemStream () {
	flush ();
}

//========================================================================================================================
//
//========================================================================================================================
size_t MemStream::write (uint8_t byte) {
	size_t ret = 1;
	if (_buffer_overflow) {
		if (FileStorage::setPosFile (_pos_write, (File &) *_fileStream))
			ret = _fileStream->write (byte);
		else
			return -1;
	}
	else if (_pos_write >= BUF_MAX_LEN) {

		if (_pos_read > 0) {
			for (int i=0; i<_pos_write-_pos_read; i++) {
				_buffer[i] = _buffer[i + _pos_read];
			}
			_pos_write = _pos_write-_pos_read;
			_pos_read = 0;
		}
		else {
			_buffer_overflow = true;

			File tmp = FileStorage::createTmpFile ();
			if (!tmp) return -1;

			_fileStream = new File (tmp);

			// Copy the buffer values in the file
			if (_fileStream->write (_buffer, BUF_MAX_LEN) != BUF_MAX_LEN) {
				Logln(F("Cannot copy the values of the memory stream in the temporary file :("));
				return -1;
			}
			_pos_read = 0;
			_pos_write = BUF_MAX_LEN;
		}
		return write (byte);
	}
	else {
		_buffer[_pos_write] = byte;
	}
	++_pos_write;
	return ret;
}

//========================================================================================================================
//
//========================================================================================================================
size_t MemStream::write (const uint8_t *buf, size_t size) {
	size_t ret = -1;
	if (_buffer_overflow) {
		if (FileStorage::setPosFile (_pos_write, (File &) *_fileStream)) {
			ret = _fileStream->write (buf, size);
			if (ret > 0) {
				_pos_write += ret;
			}
		}
	}
	else {
		ret = Print::write (buf, size);
	}
	return ret;
}

//========================================================================================================================
//
//========================================================================================================================
size_t MemStream::writeTo (Stream & stream) {
	size_t size = 0;
	while (available()) {
		size += stream.write (read());
	}
	return size;
}

//========================================================================================================================
//
//========================================================================================================================
size_t MemStream::readFrom (Stream & stream) {
	size_t size = 0;
	while (stream.available()) {
		size += write (stream.read());
	}
	return size;
}

//========================================================================================================================
//
//========================================================================================================================
void MemStream::flush () {
	if (_buffer_overflow) {
		FileStorage::deleteFile ((File &) *_fileStream);
		delete (_fileStream);
		_fileStream = NULL;

		_buffer_overflow = false;
	}
	memset(_buffer, 0, BUF_MAX_LEN * sizeof (_buffer[0]));
	_pos_write = 0;
	_pos_read = 0;
}

//========================================================================================================================
//
//========================================================================================================================
int MemStream::read () {
	char result = -1;
	if (_pos_read >= _pos_write) {
		flush ();
		return -1;
	}
	else if (_buffer_overflow) {
		if (FileStorage::setPosFile (_pos_read, (File &) *_fileStream))
			result = _fileStream->read();
		else
			return -1;
	}
	else if (_pos_read >= BUF_MAX_LEN) {
		return -1;
	}
	else {
		result = _buffer[_pos_read];
	}
	_pos_read++;
	return result;
}

//========================================================================================================================
//
//========================================================================================================================
int MemStream::peek () {
	char result = -1;
	if (_pos_read >= _pos_write) {
		flush ();
		return -1;
	}
	else if (_buffer_overflow) {
		if (FileStorage::setPosFile (_pos_read, (File &) *_fileStream))
			result = _fileStream->peek();
		else
			return -1;
	}
	else if (_pos_read >= BUF_MAX_LEN) {
		return -1;
	}
	else {
		result = _buffer[_pos_read];
	}
	return result;
}

//========================================================================================================================
//
//========================================================================================================================
int MemStream::available () {
	int ret = _pos_write-_pos_read;
	if (ret<=0) {
		ret=0;
		flush ();
	}
	return ret;
}

}

