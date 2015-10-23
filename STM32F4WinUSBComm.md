# Introduction #

This is detailed information about USB communication with STM32F4 via WinUsb driver. Firmware supports MS OS Descriptors to avoid using any Windows side driver information files. In other words device gets installed automatically when plugged in USB.


# Details #

WinUSB deatails are posted as [blog post](http://searchingforbit.blogspot.com/2014/05/winusb-communication-with-stm32-round-2.html). More detailed project information is listed here.

It's an Eclipse based CDT project. GCC compiler build by ARM was used. Path to compiler should be specified as project variable. Eclipse project files are located in separate subfolder without any sources. All sources are added in project virtual folders with relative paths. This has a disadvantage of Subclipse not picking up files in virtual folders (learned this after setting up the project).
EclipseSWorkarund.asm including startup\_stm32f407xx.s is added to project to get around Eclipse CDT not recognizing assembler files with lower case .s file extension as source files.
buildWorkarunds.c with empty void libc\_init\_array() function is added for project to link.
Project is linked with STM32F4 library build in separate project to avoid long rebuilds of HAL library provided by ST. HAL library needs couple of symbols for HW configuration like HSE clock frequency. They are defined as external functions in library and must be implemented in code using the library.

WinUSB communication is done almost like described in [STM32WinUSBCommLibrary](STM32WinUSBCommLibrary.md). The difference is that reading status from device over control endpoint is omitted. It is understood that device has finished processing the packet after return size changes to non zero.

Additional difference is in callback redesign. There is no more 'process packet' callback. Instead 'receive process' and 'transmit process' functions are called from application with packet processing in between.

USB communication is implemented as Comm Layer. This is not relevant to the project since all Comm calls basically fall through to USB implementation. Project uses this approach for future upgrades.