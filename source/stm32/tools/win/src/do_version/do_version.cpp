// do_version.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <filesystem>
#include <regex>

using namespace std;

string ReplaceAll(string str, const string& from, const string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

int FirmwareFlag(string flag, string path)
{
	int result = 0;
	bool found = false;

	// Regex to find the flag line
	regex optionRegex ("^[ \t]*bool[ \t]+firmwareFlag_" + flag + ".*$");
	
	smatch m;

	// Stream for the file
	ifstream file(path);

	// Iterate through the file to find the flag we're interested in
	string line;
	while (getline(file, line) && !found) {
		
		if (regex_search(line, m, optionRegex)) {
			result = 1;
			found = true;
		}
	}

	return result;
}

string FirmwareChannelOrder(string path)
{
	bool found = false;
	string result = "";

	// Regex to find the channel order flag line
	regex optionRegex("^[ \t]*bool[ \t]+firmwareFlag_ChannelOrder_([A-Z]{4}).*$");

	smatch m;

	// Stream for the file
	ifstream file(path);

	// Iterate through the file to find the flag we're interested in
	string line;
	while (getline(file, line) && !found) {

		if (regex_search(line, m, optionRegex)) {
			result = m[1];
			found = true;
		}
	}

	return result;
}

string ChannelOrderToBits(string channels)
{
	string result = "";
	if (channels == "AETR")
		result = "00000";
	if (channels == "AERT")
		result = "00001";
	if (channels == "ARET")
		result = "00010";
	if (channels == "ARTE")
		result = "00011";
	if (channels == "ATRE")
		result = "00100";
	if (channels == "ATER")
		result = "00101";
	if (channels == "EATR")
		result = "00110";
	if (channels == "EART")
		result = "00111";
	if (channels == "ERAT")
		result = "01000";
	if (channels == "ERTA")
		result = "01001";
	if (channels == "ETRA")
		result = "01010";
	if (channels == "ETAR")
		result = "01011";
	if (channels == "TEAR")
		result = "01100";
	if (channels == "TERA")
		result = "01101";
	if (channels == "TREA")
		result = "01110";
	if (channels == "TRAE")
		result = "01111";
	if (channels == "TARE")
		result = "10000";
	if (channels == "TAER")
		result = "10001";
	if (channels == "RETA")
		result = "10010";
	if (channels == "REAT")
		result = "10011";
	if (channels == "RAET")
		result = "10100";
	if (channels == "RATE")
		result = "10101";
	if (channels == "RTAE")
		result = "10110";
	if (channels == "RTEA")
		result = "10111";

	return result;
}

string GetDefineValue(string option, string path)
{
	string result;

	string line;

	// Regex to find the #define line for the option
	regex optionRegex("^\\s*#define\\s+" + option + "(?:[ \\t]+)([\\w \\t]+)(\\/\\/.*)?$");

	smatch m;

	// Stream for the config file
	ifstream file(path);

	// Iterate through the config file to find the config options we're interested in
	while (getline(file, line)) {

		if (regex_search(line, m, optionRegex)) {
			result = m[1];
		}
	}

	return result;
}

long BitsToDec(string bits)
{
	unsigned long value = 0;
	int indexCounter = 0;
	for (int i = bits.length() - 1; i >= 0; i--) {
		if (bits[i] == '1') {
			value += (unsigned long)pow(2, indexCounter);
		}
		indexCounter++;
	}
	return value;
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
	string buildPath = ReplaceAll(argv[1],"\\\\","\\");

	// Name of the Arduino IDE project.
	string projectName = argv[2];

	// Path to the source files.
	string sketchPath = ReplaceAll(argv[3],"\\\\","\\");

	// Arduino IDE Board name.
	string multiBoard = argv[4];

	// Flag indicating whether or not the user selected to export the binary.
	int16_t exportFlag = 0;

	if (argc == 6 && string(argv[5]) == "EXPORT")
	{
		exportFlag = 1;
	}
	
	// The type of Multi board (avr, stm, or orx).
	string multiType;
	string multiTypeBinString;

	// Set the board type and file extension for AVR boards
	if (multiBoard.find("MULTI_NO_BOOT=") == 0 || multiBoard.find("MULTI_FLASH_FROM_TX=") == 0)
	{
		multiType = "avr";
		multiTypeBinString = "00";
	}
	// Set the board type and file extension for STM32 boards
	else if (multiBoard.find("MULTI_STM32_NO_BOOT=") == 0 || multiBoard.find("MULTI_STM32_WITH_BOOT=") == 0 || multiBoard == "MULTI_STM32_FLASH_FROM_TX=")
	{
		multiType = "stm";
		multiTypeBinString = "01";
	}
	// Set the board type and file extension for OrangeRX boards
	else if (multiBoard.find("MULTI_ORANGERX=") == 0)
	{
		multiType = "orx";
		multiTypeBinString = "10";
	}
	// Throw an error and quit for an unknown board
	else
	{
		fprintf(stderr, "ERROR: Unknown board specified\n");
		return -1;
	}
	
	// Major version number
	string versionMajor;

	// Minor version number
	string versionMinor;

	// Revision number
	string versionRevision;

	// Patch level
	string versionPatch;

	// Path to the source file where we'll find the version numbers
	string versionPath = buildPath + "\\sketch\\Multiprotocol.h";

	// Error if the source file doesn't exist
	if (!filesystem::exists(versionPath)) {
		fprintf(stdout, "ERROR: %s does not exist\n", versionPath.c_str());
		return -1;
	}

	// Path to the preproc file
	string preprocPath = buildPath + "\\preproc\\ctags_target_for_gcc_minus_e.cpp";

	// Error if the source file doesn't exist
	if (!filesystem::exists(preprocPath)) {
		fprintf(stdout, "ERROR: %s does not exist\n", preprocPath.c_str());
		return -1;
	}

	// Stream for the file with version numbers
	ifstream versionFile(versionPath);
	
	// Line in file
	string line;

	versionMajor = GetDefineValue("VERSION_MAJOR", versionPath);
	versionMinor = GetDefineValue("VERSION_MINOR", versionPath);
	versionRevision = GetDefineValue("VERSION_REVISION", versionPath);
	versionPatch = GetDefineValue("VERSION_PATCH_LEVEL", versionPath);

	// The concatenated version number string
	string multiVersion = versionMajor + "." + versionMinor + "." + versionRevision + "." + versionPatch;

	// fprintf(stdout, "Firmware version: %s\n", multiVersion.c_str());

	// Get the channel order as a bit string
	string channelOrderString = FirmwareChannelOrder(preprocPath);
	string channelOrderBits = ChannelOrderToBits(channelOrderString);

	// Binary values the for config lines we're interested in
	int checkForBootloader = FirmwareFlag("CHECK_FOR_BOOTLOADER", preprocPath);
	int invertTelemetry = FirmwareFlag("INVERT_TELEMETRY", preprocPath);
	int multiStatus = FirmwareFlag("MULTI_STATUS", preprocPath);
	int multiTelemetry = FirmwareFlag("MULTI_TELEMETRY", preprocPath);
	int debugSerial = FirmwareFlag("DEBUG_SERIAL", preprocPath);

	string bootloaderSupport = "0";
	if (multiBoard.find("MULTI_FLASH_FROM_TX=") == 0 || multiBoard.find("MULTI_STM32_WITH_BOOT=") == 0)
	{
		bootloaderSupport = "1";
	}

	// Assemble the binary string of configuration options and features for the signature
	string multiSignatureFlagsBinString = to_string(debugSerial) + to_string(multiTelemetry) + to_string(multiStatus) + to_string(invertTelemetry) + to_string(checkForBootloader) + bootloaderSupport + channelOrderBits + multiTypeBinString;

	// Convert the binary string to a decimal number
	unsigned long multiSignatureFlagsDec = BitsToDec(multiSignatureFlagsBinString);

	// Char array to store the hex
	char multiSignatureFlagsHex[9];

	// Null-terminate the new array before we use it
	multiSignatureFlagsHex[0] = '\0';

	// Convert the decimal to hex; populate the array
	_itoa_s(multiSignatureFlagsDec, multiSignatureFlagsHex, 16);

	// Convert the hex value array to a string
	string multiSignatureFlagsHexString(multiSignatureFlagsHex);
	
	// Pad the hex string to 8 characters
	multiSignatureFlagsHexString = string(8 - multiSignatureFlagsHexString.length(), '0') + multiSignatureFlagsHexString;

	// The version for the signature
	string signatureVersionMajor = versionMajor.length() == 1 ? "0" + versionMajor : versionMajor;
	string signatureVersionMinor = versionMinor.length() == 1 ? "0" + versionMinor : versionMinor;
	string signatureVersionRevision = versionRevision.length() == 1 ? "0" + versionRevision : versionRevision;
	string signatureVersionPatch = versionPatch.length() == 1 ? "0" + versionPatch : versionPatch;
	string multiSignatureVersion = signatureVersionMajor + signatureVersionMinor + signatureVersionRevision + signatureVersionPatch;

	// Assemble the signature for the.bin file
	// Signature format is multi-x[hex vlue of option bitmask]-[firmware version]
	// Length of signature must be divisible by four
	string multiSignature = "multi-x" + multiSignatureFlagsHexString + "-" + multiSignatureVersion;

	//fprintf(stdout, "Firmware signature: %s\n", multiSignature.c_str());

	// Filesystem paths to the default compiled firmware files in the build directory
	filesystem::path binFileSource = buildPath + "\\" + projectName + + ".bin";
	filesystem::path hexFileSource = buildPath + "\\" + projectName + +".hex";

	// Filesystem path to the firmware file with module type and version number in the name
	filesystem::path binFileDest = buildPath + "\\multi-" + multiType + "-" + multiVersion + ".bin";
	filesystem::path hexFileDest = buildPath + "\\multi-" + multiType + "-" + multiVersion + ".hex";

	// Create the versioned bin file if the source file exists
	if (filesystem::exists(binFileSource))
	{
		// Only write the signature if we're not exporting (otherwise it gets written twice)
		if (!exportFlag)
		{
			// Append the signature in the bin file, if it exists
			ofstream outfile;
			outfile.open(binFileSource, ios_base::app);
			outfile << multiSignature;
			outfile.close();
		}

		// Copy the firmware file built by the IDE to the versioned name (in the build directory)
		filesystem::copy_file(binFileSource, binFileDest, filesystem::copy_options::overwrite_existing);
	}

	// Create the versioned hex file if the source file exists
	if (filesystem::exists(hexFileSource))
	{
		// Copy the firmware file built by the IDE to the versioned name (in the build directory)
		filesystem::copy_file(hexFileSource, hexFileDest, filesystem::copy_options::overwrite_existing);
	}

	// If we're exporting do a few extra steps
	if (exportFlag)
	{
		// Filesystem path to the default export file created by the IDE in the build path
		filesystem::path binExportPath = sketchPath + "\\multi-" + multiType + ".bin";
		filesystem::path hexExportPath = sketchPath + "\\multi-" + multiType + ".hex";
		
		// Filesystem path for the export file with module type and version number we will export to the sketch path
		filesystem::path binExportDest = sketchPath + "\\multi-" + multiType + "-" + multiVersion + ".bin";
		filesystem::path hexExportDest = sketchPath + "\\multi-" + multiType + "-" + multiVersion + ".hex";
		
		// Remove existing versioned files
		if (filesystem::exists(binExportDest))
		{
			filesystem::remove(binExportDest);
		}
		if (filesystem::exists(hexExportDest))
		{
			filesystem::remove(hexExportDest);
		}

		// Rename the export file created by the IDE in the sketch directory to the versioned name
		if (filesystem::exists(binExportPath))
		{
			filesystem::rename(binExportPath, binExportDest);
		}
		if (filesystem::exists(hexExportPath))
		{
			filesystem::rename(hexExportPath, hexExportDest);
		}

		// Tell the user what we exported
		if (filesystem::exists(binExportDest) && filesystem::exists(hexExportDest))
		{
			fprintf(stdout, "\nCompiled firmware exported as \"%s\" and \"%s\"\n", binExportDest.string().c_str(), hexExportDest.string().c_str());
		}
		else
		{
			if (filesystem::exists(binExportDest))
			{
				fprintf(stdout, "\nCompiled firmware exported as \"%s\"\n", binExportDest.string().c_str());
			}
			if (filesystem::exists(hexExportDest))
			{
				fprintf(stdout, "\nCompiled firmware exported as \"%s\"\n", hexExportDest.string().c_str());
			}
		}
	}
}
