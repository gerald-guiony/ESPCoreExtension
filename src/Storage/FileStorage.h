//************************************************************************************************************************
// FileStorage.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

//#include <FS.h>
#include <LittleFS.h>

#include <Print.h>


#define MIN_REMAINING_BYTES						0xFF

#define CHECK_IF_SPIFFS_FORMATTED_NAMEFILE		"/foo.txt"
#define TMP_NAMEFILE_PREFIX						"/tmp"
#define TMP_NAMEFILE_SUFFIX						".dat"


namespace corex {

//------------------------------------------------------------------------------
//
class FileStorage
{

public:
	static void init								();

	static void spiffsMountFileSystem				();
	static void spiffsCheckIfFormatted				();
	static String spiffsListFiles					();
	static void spiffsInfos							();
	static size_t spiffsTotalBytes					();
	static size_t spiffsRemainingBytes				();
	static bool spiffsCheckRemainingBytes			();
	static void spiffsRemoveAllFiles				();
	static void spiffsRemoveAllTmpFiles				();

	static bool readTextFile						(const String & filename, String & text);
	static bool writeTextFile						(const String & filename, const String & text);
	static bool printTextFile						(const String & filename, Print & printer);

	static File createFile							(const String & filename);
	static void deleteFile							(const String & filename);
	static void deleteFile							(File & file);

	static bool isFileExists						(const String & filename);
	static bool setPosFile							(int pos, File & file);

	static File createTmpFile						();
};

}










