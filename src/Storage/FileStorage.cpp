//************************************************************************************************************************
// FileStorage.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>
#include <Stream.h>
#include <StreamString.h>

#include "Print/Logger.h"
#include "FileStorage.h"

namespace corex {

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: init ()
{
	spiffsMountFileSystem ();
	spiffsCheckIfFormatted ();
	spiffsRemoveAllTmpFiles ();
	//spiffsRemoveAllFiles ();							// Reset spiffs - for testing
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: spiffsMountFileSystem ()
{
	if (!LittleFS.begin())								// always use this to "mount" the filesystem
	{
		Logln(F("ERROR : Can't open the LittleFS..."));
		return;
	}
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: spiffsCheckIfFormatted ()
{
	File f = LittleFS.open(CHECK_IF_SPIFFS_FORMATTED_NAMEFILE, "w+");
	if (f) {
		f.close();
	}
	else {
		// Next lines have to be done ONLY ONCE!!!!!When LittleFS is formatted ONCE you can comment these lines out!!
		Logln(F("Please wait 30 secs for LittleFS to be formatted..."));
		LittleFS.format();
		Logln(F("OK! Spiffs formatted"));
	}
}

//========================================================================================================================
//
//========================================================================================================================
String FileStorage :: spiffsListFiles ()
{
	StreamString sstr;

#ifdef ESP8266

	Dir dir = LittleFS.openDir("/");
	while (dir.next()) {
		sstr << dir.fileName() << F(" (") << dir.fileSize() << F(" bytes), ");
		Logln(dir.fileName() << F("\t\t") << dir.fileSize() << F(" bytes"));
	}

#elif defined (ESP32)

	File root = LittleFS.open("/", "r");

	if (!root) {
		Logln("- Fails to open root directory");
		return sstr;
	}

	File file = root.openNextFile();

	while (file) {

		sstr << file.name() << F(" (") << file.size() << F(" bytes), ");
		Logln(file.name() << F("\t\t") << file.size() << F(" bytes"));

		file = root.openNextFile();
	}

#endif

	return sstr;
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: spiffsInfos ()
{
	spiffsListFiles ();

#ifdef ESP8266

	FSInfo fs_info;
	LittleFS.info(fs_info);

	size_t total = fs_info.totalBytes;
	size_t used = fs_info.usedBytes;

#elif defined (ESP32)

	size_t total = LittleFS.totalBytes();
	size_t used = LittleFS.usedBytes();

#endif

	Logln(
		F("Spiffs total bytes : ")		<< total	<< LN <<
		F("Spiffs used bytes : ")		<< used		<< LN <<
		F("Spiffs remaining bytes : ")	<< (total - used)
	);

}

//========================================================================================================================
//
//========================================================================================================================
bool FileStorage :: spiffsCheckRemainingBytes ()
{
	size_t result = 0;

#ifdef ESP8266

	FSInfo fs_info;
	LittleFS.info(fs_info);

	result = fs_info.totalBytes - fs_info.usedBytes;

#elif defined (ESP32)

	result = LittleFS.totalBytes() - LittleFS.usedBytes();

#endif

	if (result < MIN_REMAINING_BYTES) {
		Logln(F("*** WARNING : available spiffs space is too low !"));
		return false;
	}

	return true;
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: spiffsRemoveAllFiles ()
{
#ifdef ESP8266

	Dir dir = LittleFS.openDir("/");
	while (dir.next()) {
		LittleFS.remove (dir.fileName());
	}

#elif defined (ESP32)

	File root = LittleFS.open("/", "r");
	File file = root.openNextFile();
	while (file) {
		LittleFS.remove (file.name());
		file = root.openNextFile();
	}

#endif

	Logln(F("All spiffs files were removed!"));
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: spiffsRemoveAllTmpFiles ()
{
#ifdef ESP8266
//	Logln(F("Removing tmp files :"));
	Dir dir = LittleFS.openDir("/");
	while (dir.next()) {
		if (dir.fileName().startsWith(F(TMP_NAMEFILE_PREFIX))) {
			Logln(F("Removing tmp file: ") << dir.fileName());
			LittleFS.remove (dir.fileName());
		}
	}
#elif defined (ESP32)

	File root = LittleFS.open("/", "r");
	File file = root.openNextFile();
	while (file) {
		String fileName = file.name();
		if (fileName.startsWith(F(TMP_NAMEFILE_PREFIX))) {
			Logln(F("Removing tmp file: ") << file.name());
			LittleFS.remove (file.name());
		}
		file = root.openNextFile();
	}

#endif
}

//========================================================================================================================
//
//========================================================================================================================
File FileStorage :: createFile (const String & filename)
{
	File f = LittleFS.open (filename, "w+");
	if (!f) {
		Logln(F("ERROR : Can't create the file : ") << filename);
		return f;
	}

	Logln(F("The file '") << filename << F("' is created"));

	return f;
}

//========================================================================================================================
//
//========================================================================================================================
File FileStorage :: createTmpFile ()
{
	static int i = 0;

	StreamString sstr;
	sstr << F(TMP_NAMEFILE_PREFIX) << i << F(TMP_NAMEFILE_SUFFIX);

	i++;
	if (i >= 10) i = 0;			// 10 temporary files

	return createFile (sstr.c_str());
}

//========================================================================================================================
//
//========================================================================================================================
bool FileStorage :: printTextFile (const String & filename, Print & printer) {

	File f = LittleFS.open(filename, "r");
	if (!f) {
		Logln(F("Warning : Can't open the file : ") << filename);
		return false;
	}

	printer << F("Read file '") << filename << F("' :") << LN;
	while (f.available()){
		printer << (char)f.read();
	}
	printer << LN;

	f.close ();
	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool FileStorage :: readTextFile (const String & filename, String & text)
{
	File f = LittleFS.open(filename, "r");
	if (!f) {
		Logln(F("Warning : Can't open the file : ") << filename);
		return false;
	}
	text = f.readStringUntil('\r');
	f.close ();

	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool FileStorage :: writeTextFile (const String & filename, const String & text)
{
	File f = LittleFS.open(filename, "w");
	if (!f) {
		Logln(F("ERROR : Can't create the file : ") << filename);
		return false;
	}
	f.println (text.c_str());
	f.close ();

	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool FileStorage :: isFileExists (const String & filename)
{
	File f = LittleFS.open(filename, "r");
	if (!f) {
		return false;
	}
	f.close ();
	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool FileStorage :: setPosFile (int pos, File & file)
{
	return file.seek(pos, SeekMode::SeekSet);
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: deleteFile (const String & filename)
{
	if (isFileExists (filename)) {
		LittleFS.remove (filename);
		Logln(F("The file '") << filename << F("' was deleted"));
	}
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: deleteFile (File & file)
{
	spiffsInfos ();

	String filename = file.name();
	file.close();

	deleteFile (filename.c_str());
}

}