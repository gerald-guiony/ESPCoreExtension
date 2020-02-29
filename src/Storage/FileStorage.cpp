//************************************************************************************************************************
// FileStorage.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#ifdef ESP8266

#include <Arduino.h>
#include <Stream.h>
#include <StreamString.h>

#include "Print/Logger.h"
#include "FileStorage.h"


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
	if (!SPIFFS.begin())								// always use this to "mount" the filesystem
	{
		Logln(F("ERROR : Can't open the SPIFFS..."));
		return;
	}
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: spiffsCheckIfFormatted ()
{
	File f = SPIFFS.open(CHECK_IF_SPIFFS_FORMATTED_NAMEFILE, "w+");
	if (f) {
		f.close();
	}
	else {
		// Next lines have to be done ONLY ONCE!!!!!When SPIFFS is formatted ONCE you can comment these lines out!!
		Logln(F("Please wait 30 secs for SPIFFS to be formatted..."));
		SPIFFS.format();
		Logln(F("OK! Spiffs formatted"));
	}
}

//========================================================================================================================
//
//========================================================================================================================
String FileStorage :: spiffsListFiles ()
{
	StreamString sstr;
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		sstr << dir.fileName() << F(" (") << dir.fileSize() << F(" bytes), ");
		Logln(dir.fileName() << F("\t\t") << dir.fileSize() << F(" bytes"));
	}
	return sstr;
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: spiffsInfos ()
{
	spiffsListFiles ();

	FSInfo fs_info;
	SPIFFS.info(fs_info);

	Logln(
		F("Spiffs total bytes : ")		<< fs_info.totalBytes						<< LN <<
		F("Spiffs used bytes : ")		<< fs_info.usedBytes						<< LN <<
		F("Spiffs remaining bytes : ")	<< (fs_info.totalBytes - fs_info.usedBytes)
	);
}

//========================================================================================================================
//
//========================================================================================================================
bool FileStorage :: spiffsCheckRemainingBytes ()
{
	size_t result = 0;

	FSInfo fs_info;
	SPIFFS.info(fs_info);
	result = fs_info.totalBytes - fs_info.usedBytes;

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
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		SPIFFS.remove (dir.fileName());
	}
	Logln(F("All spiffs files were removed!"));
}

//========================================================================================================================
//
//========================================================================================================================
void FileStorage :: spiffsRemoveAllTmpFiles ()
{
//	Logln(F("Removing tmp files :"));
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		if (dir.fileName().startsWith(F(TMP_NAMEFILE_PREFIX))) {
			Logln(F("Removing tmp file: ") << dir.fileName());
			SPIFFS.remove (dir.fileName());
		}
	}
}

//========================================================================================================================
//
//========================================================================================================================
File FileStorage :: createFile (const String & filename)
{
	File f = SPIFFS.open (filename, "w+");
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
	
	File f = SPIFFS.open(filename, "r");
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
	File f = SPIFFS.open(filename, "r");
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
	File f = SPIFFS.open(filename, "w");
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
	File f = SPIFFS.open(filename, "r");
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
		SPIFFS.remove (filename);
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


#endif
