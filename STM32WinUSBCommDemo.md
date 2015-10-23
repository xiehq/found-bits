# Driver #
For a Windows driver setup run the Driver.py script. It will create a self-signed certificate, the INF and CAT files and sign them. You need to download and install the Windows Driver Kit (WDK). For more information about driver signing read [this](http://searchingforbit.blogspot.com/2012/04/winusb-communication-with-stm32-part-3.html).

# Firmware #
Project is made in Eclipse. The CDT internal builder is used. It depends on files from ST libraries and it is based on Mass storage USB example from ST. I kept the file names and layout as ST does in their examples. Codesourcery Lite compiler for ARM is used to build the project. Development and testing was done on STM32F103RB device.
[Some additional info](http://searchingforbit.blogspot.com/2012/04/winusb-communication-with-stm32-part-1.html)

### To build the project: ###
  1. Add USB\_VID and USB\_PID macros to compiler symbols or create _usbID.h_ in project folder and assign macros there:
```
   #ifndef __USB_ID_H__
   #define __USB_ID_H__

   #define USB_VID <your USB VID here>
   #define USB_PID <your USB PID here>
   #define USB_VENDORNAME <your name here>

   #endif  //  __USB_ID_H__
```

> or (preferably) run the .\Windows\Driver.py to create it.
  1. Run PreBuildOnce.py to get some files from original sample.
  1. Refresh the project.
  1. Build.

# Software #
Windows Driver Kit (WDK) must be installed to build the test project. Additional include folders from WDK must be added, additional link folders and libraries must be added. There was a syntax error with some WDK header file inclusion. That is why WDK include folders are added to VC++ Include Directories after default include path.
![https://found-bits.googlecode.com/svn/wiki/WinUSBCommTestSettings.jpg](https://found-bits.googlecode.com/svn/wiki/WinUSBCommTestSettings.jpg)