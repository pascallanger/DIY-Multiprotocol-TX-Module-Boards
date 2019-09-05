// do_version.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <filesystem>

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

int main(int argc, char *argv[])
{
	// Error if the number of arguments is wrong - we expect four or five
	if (argc < 5 || argc > 6)
	{
		fprintf(stderr, "ERROR: Incorrect number of arguments\n");
		return -1;
	}

	// Path of the build directory.
	std::string buildPath = ReplaceAll(argv[1],"\\\\","\\");

	// Name of the Arduino IDE project.
	std::string projectName = argv[2];

	// Path to the source files.
	std::string sketchPath = ReplaceAll(argv[3],"\\\\","\\");

	// Arduino IDE Board name.
	std::string multiBoard = argv[4];

	// Flag indicating whether or not the user selected to export the binary.
	std::int16_t exportFlag = 0;

	if (argc == 6 && std::string(argv[5]) == "EXPORT")
	{
		exportFlag = 1;
	}
	
	// The type of Multi board (avr, stm, or orx).
	std::string multiType;

	// The file extension which the compiled file will have (.hex or .bin).
	std::string multiExtension;

	// Set the board type and file extension for AVR boards
	if (multiBoard.find("MULTI_NO_BOOT=") == 0 || multiBoard.find("MULTI_FLASH_FROM_TX=") == 0)
	{
		multiType = "avr";
		multiExtension = ".hex";
	}
	// Set the board type and file extension for STM32 boards
	else if (multiBoard.find("MULTI_STM32_NO_BOOT=") == 0 || multiBoard.find("MULTI_STM32_WITH_BOOT=") == 0 || multiBoard == "MULTI_STM32_FLASH_FROM_TX=")
	{
		multiType = "stm";
		multiExtension = ".bin";
	}
	// Set the board type and file extension for OrangeRX boards
	else if (multiBoard.find("MULTI_ORANGERX=") == 0)
	{
		multiType = "orx";
		multiExtension = ".hex";
	}
	// Throw an error and quit for an unknown board
	else
	{
		fprintf(stderr, "ERROR: Unknown board specified\n");
		return -1;
	}
	
	// Major version number
	std::string versionMajor;

	// Minor version number
	std::string versionMinor;

	// Revision number
	std::string versionRevision;

	// Patch level
	std::string versionPatch;

	// Path to the source file where we'll find the version numbers
	std::string versionPath = buildPath + "\\sketch\\Multiprotocol.h";

	// Stream for the file with version numbers
	std::ifstream versionFile(versionPath);
	
	// Line in file
	std::string line;

	// Iterate through the version source file to find the lines containing elements of the version number
	while (getline(versionFile, line)) {

		// Find the major version number and get the number from the end of the line
		if (line.find("#define VERSION_MAJOR") >= 0)
		{
			std::string tmp = line.substr(line.find_last_of(" \t"));
			versionMajor = tmp.substr(tmp.find_first_not_of(" \t"));
		}

		// Find the minor version number and get the number from the end of the line
		if (line.find("#define VERSION_MINOR") >= 0)
		{
			std::string tmp = line.substr(line.find_last_of(" \t"));
			versionMinor = tmp.substr(tmp.find_first_not_of(" \t"));
		}

		// Find the revision number and get the number from the end of the line
		if (line.find("#define VERSION_REVISION") >= 0)
		{
			std::string tmp = line.substr(line.find_last_of(" \t"));
			versionRevision = tmp.substr(tmp.find_first_not_of(" \t"));
		}

		// Find the patch level and get the number from the end of the string
		if (line.find("#define VERSION_PATCH_LEVEL") >= 0)
		{
			std::string tmp = line.substr(line.find_last_of(" \t"));
			versionPatch = tmp.substr(tmp.find_first_not_of(" \t"));
		}
	}

	// The concatenated version number string
	std::string multiVersion = versionMajor + "." + versionMinor + "." + versionRevision + "." + versionPatch;

	// Filesystem path to the default compiled firmware file in the build directory
	std::filesystem::path src = buildPath + "\\" + projectName + multiExtension;

	// Filesystem path to the firmware file with module type and version number in the name
	std::filesystem::path dest = buildPath + "\\multi-" + multiType + "-" + multiVersion + multiExtension;

	// Check if the source file exists
	if (std::filesystem::exists(src))
	{
		// Copy the firmware file built by the IDE to the versioned name (in the build directory)
		std::filesystem::copy_file(src, dest, std::filesystem::copy_options::overwrite_existing);
	}

	// If we're exporting do a few extra steps
	if (exportFlag)
	{
		// Filesystem path to the default export file created by the IDE in the build path
		std::filesystem::path exportPath = buildPath + "\\multi-" + multiType + multiExtension;
		
		// Remove the build path export file if it exists
		if (std::filesystem::exists(exportPath))
		{
			std::filesystem::remove(exportPath);
		}

		// Filesystem path to the export file created by the IDE in the sketch directory
		std::filesystem::path exportSketchPath = sketchPath + "\\multi-" + multiType + multiExtension;

		// Filesystem path for the export file with module type and version number in teh sketch directory
		std::filesystem::path exportDestPath = sketchPath + "\\multi-" + multiType + "-" + multiVersion + multiExtension;

		// Remove a versioned firmware file if one exists in the sketch directory
		if (std::filesystem::exists(exportDestPath))
		{
			std::filesystem::remove(exportDestPath);
		}

		// Rename the export file created by the IDE in the sketch directory to the versioned name
		if (std::filesystem::exists(exportSketchPath))
		{
			std::filesystem::rename(exportSketchPath,exportDestPath);
		}
	}
}
