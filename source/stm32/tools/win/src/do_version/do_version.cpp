// do_version.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <filesystem>
#include <regex>

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

bool FirmwareFlag(std::string flag, std::string path)
{
	bool result = false;

	// Regex to find the flag line
	std::regex optionRegex ("^[ \t]*uint8_t[ \t]+firmwareFlag_" + flag + ".*$");
	
	std::smatch m;

	// Stream for the file
	std::ifstream file(path);

	// Iterate through the file to find the flag we're interested in
	std::string line;
	while (getline(file, line) && !result) {
		
		if (std::regex_search(line, m, optionRegex)) {
			result = true;
		}
	}

	return result;
}

std::string getDefineValue(std::string option, std::string path)
{
	std::string result;

	std::string line;

	// Regex to find the #define line for the option
	std::regex optionRegex("^\\s*#define\\s+" + option + "(?:[ \\t]+)([\\w \\t]+)(\\/\\/.*)?$");

	std::smatch m;

	// Stream for the config file
	std::ifstream file(path);

	// Iterate through the config file to find the config options we're interested in
	while (getline(file, line)) {

		if (std::regex_search(line, m, optionRegex)) {
			result = m[1];
		}
	}

	return result;
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

	// Set the board type and file extension for AVR boards
	if (multiBoard.find("MULTI_NO_BOOT=") == 0 || multiBoard.find("MULTI_FLASH_FROM_TX=") == 0)
	{
		multiType = "avr";
	}
	// Set the board type and file extension for STM32 boards
	else if (multiBoard.find("MULTI_STM32_NO_BOOT=") == 0 || multiBoard.find("MULTI_STM32_WITH_BOOT=") == 0 || multiBoard == "MULTI_STM32_FLASH_FROM_TX=")
	{
		multiType = "stm";
	}
	// Set the board type and file extension for OrangeRX boards
	else if (multiBoard.find("MULTI_ORANGERX=") == 0)
	{
		multiType = "orx";
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

	// Error if the source file doesn't exist
	if (!std::filesystem::exists(versionPath)) {
		fprintf(stdout, "ERROR: %s does not exist\n", versionPath.c_str());
		return -1;
	}

	// Path to the preproc file
	std::string preprocPath = buildPath + "\\preproc\\ctags_target_for_gcc_minus_e.cpp";

	// Error if the source file doesn't exist
	if (!std::filesystem::exists(preprocPath)) {
		fprintf(stdout, "ERROR: %s does not exist\n", preprocPath.c_str());
		return -1;
	}

	// Stream for the file with version numbers
	std::ifstream versionFile(versionPath);
	
	// Line in file
	std::string line;

	versionMajor = getDefineValue("VERSION_MAJOR", versionPath);
	versionMinor = getDefineValue("VERSION_MINOR", versionPath);
	versionRevision = getDefineValue("VERSION_REVISION", versionPath);
	versionPatch = getDefineValue("VERSION_PATCH_LEVEL", versionPath);

	// The concatenated version number string
	std::string multiVersion = versionMajor + "." + versionMinor + "." + versionRevision + "." + versionPatch;

	// fprintf(stdout, "Firmware version: %s\n", multiVersion.c_str());

	// Variables for config lines we're interested in
	bool checkForBootloaderEnabled = FirmwareFlag("CHECK_FOR_BOOTLOADER", preprocPath);
	bool multiStatusEnabled = FirmwareFlag("MULTI_STATUS", preprocPath);
	bool multiTelemetryEnabled = FirmwareFlag("MULTI_TELEMETRY", preprocPath);
	bool invertTelemetryEnabled = FirmwareFlag("INVERT_TELEMETRY", preprocPath);
	bool debugSerialEnabled = FirmwareFlag("DEBUG_SERIAL", preprocPath);

	std::string flag_BOOTLOADER_SUPPORT = "u";
	if (multiBoard.find("MULTI_FLASH_FROM_TX=") == 0 || multiBoard.find("MULTI_STM32_WITH_BOOT=") == 0)
	{
		flag_BOOTLOADER_SUPPORT = "b";
	}

	std::string flag_TELEMETRY_TYPE = "u";
	if (!(multiStatusEnabled && multiTelemetryEnabled))
	{
		if (multiStatusEnabled)
		{
			flag_TELEMETRY_TYPE = "s";
		}
		if (multiTelemetryEnabled)
		{
			flag_TELEMETRY_TYPE = "t";
		}
	}

	std::string flag_CHECK_FOR_BOOTLOADER = checkForBootloaderEnabled ? "c" : "u";
	std::string flag_INVERT_TELEMTERY = invertTelemetryEnabled ? "i" : "u";
	std::string flag_DEBUG_SERIAL = debugSerialEnabled ? "d" : "u";

	// The features for the signature
	std::string multiSignatureFlags = flag_BOOTLOADER_SUPPORT + flag_CHECK_FOR_BOOTLOADER + flag_TELEMETRY_TYPE + flag_INVERT_TELEMTERY + flag_DEBUG_SERIAL;

	// The version for the signature
	std::string signatureVersionMajor = versionMajor.length() == 1 ? "0" + versionMajor : versionMajor;
	std::string signatureVersionMinor = versionMinor.length() == 1 ? "0" + versionMinor : versionMinor;
	std::string signatureVersionRevision = versionRevision.length() == 1 ? "0" + versionRevision : versionRevision;
	std::string signatureVersionPatch = versionPatch.length() == 1 ? "0" + versionPatch : versionPatch;
	std::string multiSignatureVersion = signatureVersionMajor + signatureVersionMinor + signatureVersionRevision + signatureVersionPatch;

	// Assemble the signature for the.bin file
	// Signature format is multi-[board type]-[bootloader support][check for bootloader][multi telemetry type][telemetry inversion][serial debug]-[firmware version]
	// Length of signature must be divisible by four
	std::string multiSignature = "multi-" + multiType + "-" + multiSignatureFlags + "-" + multiSignatureVersion;

	//fprintf(stdout, "Firmware signature: %s\n", multiSignature.c_str());

	// Filesystem paths to the default compiled firmware files in the build directory
	std::filesystem::path binFileSource = buildPath + "\\" + projectName + + ".bin";
	std::filesystem::path hexFileSource = buildPath + "\\" + projectName + +".hex";

	// Filesystem path to the firmware file with module type and version number in the name
	std::filesystem::path binFileDest = buildPath + "\\multi-" + multiType + "-" + multiVersion + ".bin";
	std::filesystem::path hexFileDest = buildPath + "\\multi-" + multiType + "-" + multiVersion + ".hex";

	// Create the versioned bin file if the source file exists
	if (std::filesystem::exists(binFileSource))
	{
		// Append the signature in the bin file, if it exists
		std::ofstream outfile;

		outfile.open(binFileSource, std::ios_base::app);
		outfile << multiSignature;
		outfile.close();

		// Copy the firmware file built by the IDE to the versioned name (in the build directory)
		std::filesystem::copy_file(binFileSource, binFileDest, std::filesystem::copy_options::overwrite_existing);
	}

	// Create the versioned hex file if the source file exists
	if (std::filesystem::exists(hexFileSource))
	{
		// Copy the firmware file built by the IDE to the versioned name (in the build directory)
		std::filesystem::copy_file(hexFileSource, hexFileDest, std::filesystem::copy_options::overwrite_existing);
	}

	// If we're exporting do a few extra steps
	if (exportFlag)
	{
		// Filesystem path to the default export file created by the IDE in the build path
		std::filesystem::path binExportPath = sketchPath + "\\multi-" + multiType + ".bin";
		std::filesystem::path hexExportPath = sketchPath + "\\multi-" + multiType + ".hex";
		
		// Filesystem path for the export file with module type and version number we will export to the sketch path
		std::filesystem::path binExportDest = sketchPath + "\\multi-" + multiType + "-" + multiVersion + ".bin";
		std::filesystem::path hexExportDest = sketchPath + "\\multi-" + multiType + "-" + multiVersion + ".hex";
		
		// Remove existing versioned files
		if (std::filesystem::exists(binExportDest))
		{
			std::filesystem::remove(binExportDest);
		}
		if (std::filesystem::exists(hexExportDest))
		{
			std::filesystem::remove(hexExportDest);
		}

		// Rename the export file created by the IDE in the sketch directory to the versioned name
		if (std::filesystem::exists(binExportPath))
		{
			std::filesystem::rename(binExportPath, binExportDest);
		}
		if (std::filesystem::exists(hexExportPath))
		{
			std::filesystem::rename(hexExportPath, hexExportDest);
		}

		// Tell the user what we exported
		if (std::filesystem::exists(binExportDest) && std::filesystem::exists(hexExportDest))
		{
			fprintf(stdout, "\nCompiled firmware exported as \"%s\" and \"%s\"\n", binExportDest.string().c_str(), hexExportDest.string().c_str());
		}
		else
		{
			if (std::filesystem::exists(binExportDest))
			{
				fprintf(stdout, "\nCompiled firmware exported as \"%s\"\n", binExportDest.string().c_str());
			}
			if (std::filesystem::exists(hexExportDest))
			{
				fprintf(stdout, "\nCompiled firmware exported as \"%s\"\n", hexExportDest.string().c_str());
			}
		}
	}
}
