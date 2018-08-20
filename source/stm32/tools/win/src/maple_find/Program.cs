namespace maple_find
{
    using System;
    using System.Collections.Generic;
    using System.Management;

    class Program
    {
        static int Main(string[] args)
        {
            bool verboseOutput = false;
            string outputLine = null;
            int returnCode = 0;

            if (args.Length > 0 && args[0] == "-v")
            {
                verboseOutput = true;
            }

            var usbDevices = GetUSBDevices();

            if (usbDevices.Count == 0)
            {
                outputLine = "No Maple devices found.";
                returnCode = 1;
            }

            foreach (var usbDevice in usbDevices)
            {
                switch (usbDevice.PnpDeviceID.Substring(0,21))
                {
                    case "USB\\VID_1EAF&PID_0003":
                        outputLine = "Maple DFU Device found.";
                        returnCode = 0;
                        break;
                    case "USB\\VID_1EAF&PID_0004":
                        outputLine = "Maple USB Device found.";
                        returnCode = 0;
                        break;
                    default:
                        outputLine = "Unknown Maple device found.";
                        returnCode = 1;
                        break;
                }

                Console.WriteLine("");
                Console.WriteLine(outputLine);
                if (verboseOutput)
                {
                    Console.WriteLine("");
                    Console.WriteLine("DeviceID:     {0}", usbDevice.DeviceID);
                    Console.WriteLine("Manufacturer: {0}", usbDevice.Manufacturer);
                    Console.WriteLine("Description:  {0}", usbDevice.Description);
                    Console.WriteLine("Name:         {0}", usbDevice.Name);
                    Console.WriteLine("Status:       {0}", usbDevice.Status);
                };
                Console.WriteLine("");

                return returnCode;
            }

            return returnCode;
        }

        static List<USBDeviceInfo> GetUSBDevices()
        {
            List<USBDeviceInfo> devices = new List<USBDeviceInfo>();

            ManagementObjectCollection collection;
            using (var searcher = new ManagementObjectSearcher(@"Select * From Win32_PnPEntity WHERE DeviceID like '%VID_1EAF&%'"))
                collection = searcher.Get();

            foreach (var device in collection)
            {
                devices.Add(new USBDeviceInfo(
                (string)device.GetPropertyValue("DeviceID"),
                (string)device.GetPropertyValue("PNPDeviceID"),
                (string)device.GetPropertyValue("Description"),
                (string)device.GetPropertyValue("Manufacturer"),
                (string)device.GetPropertyValue("Name"),
                (string)device.GetPropertyValue("Status")

                ));
            }

            collection.Dispose();
            return devices;
        }
    }

    class USBDeviceInfo
    {
        public USBDeviceInfo(string deviceID, string pnpDeviceID, string description, string manufacturer, string name, string status)
        {
            this.DeviceID = deviceID;
            this.PnpDeviceID = pnpDeviceID;
            this.Description = description;
            this.Manufacturer = manufacturer;
            this.Name = name;
            this.Status = status;
        }
        public string DeviceID { get; private set; }
        public string PnpDeviceID { get; private set; }
        public string Description { get; private set; }
        public string Manufacturer { get; private set; }
        public string Name { get; private set; }
        public string Status { get; private set; }
    }
}