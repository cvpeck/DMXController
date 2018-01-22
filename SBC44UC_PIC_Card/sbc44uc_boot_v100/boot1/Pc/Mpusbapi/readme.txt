Release Notes for MPUSBAPI Library, Microchip Technology Inc.
v1.00
19 November 2004

----------------------------------------------------------------------
1. What is MPUSBAPI Library?
----------------------------------------------------------------------

* MPUSBAPI is a DLL module providing wrapper functions for the
  Microchip General Purpose USB Windows driver, mchpusb.sys.
  It abstracts out the complexity in working with Win32 API functions.

* Seven basic functions are provided in this release:

  - MPUSBGetDeviceCount

  - MPUSBOpen

  - MPUSBClose

  - MPUSBRead

  - MPUSBReadInt

  - MPUSBWrite

  - MPUSBGetDLLVersion

* Details on how to use each function are documented in the source code
  - Refer to \DLL\Borland_C\Source\_mpusbapi.cpp

----------------------------------------------------------------------
2. Linking MPUSBAPI to your project
----------------------------------------------------------------------

There are two ways to link the DLL to your project:

a) Load-time Linking

   Simply add the mpusbapi.lib file to your project.
   The mpusbapi.lib file provided is specific to the Borland C++
   compiler. The mpusbapi.dll is compiler independent. If a different
   compiler is used, however, the DLL must be re-compiled to generate
   a new mpusbapi.lib specific to that compiler.

   C++ source code for MPUSBAPI is provided.
   
   The _mpusbapi.h file must be included in every file that uses
   the MPUSBAPI functions.

   The mpusbapi.dll file must be in the same directory as the
   executable file.

b) Run-time Linking

   Run-time Linking does not require the .lib file. It only requires
   the .dll file. Linking the functions is, however, a little more
   complicated. An example is provided showing how to use Win32
   API functions to load a DLL.

   The mpusbapi.h file (note, no "_") must be included in every file
   that uses the MPUSBAPI functions.

   The mpusbapi.dll is not required to be in the same directory as
   the executable file.

