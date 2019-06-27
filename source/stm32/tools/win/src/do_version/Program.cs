using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace do_version
{
    class Program
    {
        static void Main(string[] args)
        {
            //Console.WriteLine("\ndomultiversion.exe\n");
            // We expect either two or six arguments, die if we didn't get them
            if (args.Length != 4 && args.Length != 5)
            {
                Console.WriteLine("Error: Incorrect number of arguments.");
                Environment.Exit(1);
            }

            // Get the arguments from the command line
            string buildPath = null;
            string projectName = null;
            string sketchPath = null;
            string multiBoard = null;
            bool exportFlag = false;

            buildPath = args[0];
            projectName = args[1];
            sketchPath = args[2];
            multiBoard = args[3].Split('=')[0];

            if (args.Length == 5)
            {
                if (args[4] == "EXPORT")
                {
                    exportFlag = true;
                }
            }

            // Get the board type from the board
            string multiType = null;
            string multiExtension = null;
            switch (multiBoard)
            {
                case "MULTI_NO_BOOT":
                    multiType = "avr";
                    multiExtension = ".hex";
                    break;
                case "MULTI_FLASH_FROM_TX":
                    multiType = "avr";
                    multiExtension = ".hex";
                    break;
                case "MULTI_STM32_NO_BOOT":
                    multiType = "stm";
                    multiExtension = ".bin";
                    break;
                case "MULTI_STM32_WITH_BOOT":
                    multiType = "stm";
                    multiExtension = ".bin";
                    break;
                case "MULTI_STM32_FLASH_FROM_TX":
                    multiType = "stm";
                    multiExtension = ".bin";
                    break;
                case "MULTI_ORANGERX":
                    multiType = "orx";
                    multiExtension = ".hex";
                    break;
            }

            //Console.WriteLine("Sketch Path: {0}", sketchPath);
            //Console.WriteLine("Multi Board: {0}", multiBoard);
            //Console.WriteLine("Multi Board Type: {0}", multiType);

            // Get the firmware version from the source
            string majorVersion = null;
            string minorVersion = null;
            string revisionVersion = null;
            string patchVersion = null;

            string versionPath = buildPath + "\\sketch\\Multiprotocol.h";
            try
            {
                System.IO.StreamReader fileReader = new System.IO.StreamReader(versionPath);
                string line;
                while ((line = fileReader.ReadLine()) != null)
                {
                    if (line.Contains("#define VERSION_MAJOR"))
                    {
                        string[] strings = line.Split();
                        majorVersion = strings[strings.Length - 1];
                    }
                    if (line.Contains("#define VERSION_MINOR"))
                    {
                        string[] strings = line.Split();
                        minorVersion = strings[strings.Length - 1];
                    }
                    if (line.Contains("#define VERSION_REVISION"))
                    {
                        string[] strings = line.Split();
                        revisionVersion = strings[strings.Length - 1];
                    }
                    if (line.Contains("#define VERSION_PATCH_LEVEL"))
                    {
                        string[] strings = line.Split();
                        patchVersion = strings[strings.Length - 1];
                    }
                }
            } 
            catch (FileNotFoundException) {
                Console.WriteLine("Error: Unable to open {0}.", versionPath);
                Environment.Exit(1);
            }

            // Put the version string together
            string multiVersion = null;
            if (majorVersion != null && minorVersion != null && revisionVersion != null && patchVersion != null)
            {
                multiVersion = String.Format("{0}.{1}.{2}.{3}", majorVersion, minorVersion, revisionVersion, patchVersion);
            } else {
                multiVersion = "";
            }

            //Console.WriteLine("Multi Version: {0}", multiVersion);

            // Build the file paths
            string destFile = buildPath + "\\multi-" + multiType + "-" + multiVersion + multiExtension;
            string sourceFile = buildPath + "\\" + projectName + multiExtension;

            // Copy the compiled file to the versioned name
            if (File.Exists(sourceFile))
            {
                File.Copy(sourceFile, destFile, true);
            }

            // Delete the original export file
            if (exportFlag)
            {
                string exportFile = buildPath + "\\multi-" + multiType + multiExtension;
                if (File.Exists(exportFile))
                {
                    File.Delete(exportFile);
                }
            }
        }
    }
}
