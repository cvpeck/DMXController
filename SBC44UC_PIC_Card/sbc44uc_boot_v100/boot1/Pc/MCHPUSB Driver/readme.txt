Release Notes for Microchip General Purpose USB Windows Driver,
Microchip Technology Inc.
v1.00.005
19 November 2004

----------------------------------------------------------------------
Release VS Debug
----------------------------------------------------------------------

Microchip provides two versions of the driver. One is for
production release, and one is for debugging during development. The
only difference is the debug version outputs debug messages which
can be viewed by using a 3rd party tool, such as DebugView.

DebugView can be downloaded from:

http://www.sysinternals.com/ntw2k/freeware/debugview.shtml

To enable the output messages, open the device property page from the
Device Manager, select Tracing tab and enable appropriate options.

To switch between the two versions, the driver has to be reinstalled
using a different INF file. Both INF files are provided under their
respective folders.

----------------------------------------------------------------------
USB Vendor ID and Product ID
----------------------------------------------------------------------

In order to install the driver for a particular device, the VID and
PID in the INF file must match the device VID and PID. This can be
done by modifying the two IDs in the INF file under the [DeviceList]
section. The IDs are stored as a hex number.

----------------------------------------------------------------------
Application Programming Interface
----------------------------------------------------------------------

There are two methods of using the functions provided by the driver.

1. Use MPUSBAPI DLL, this DLL provides easy to use interface to the
   driver. See MPUSBAPI directory for details.

2. Interface your Windows project directly to the driver. In this
   case, you must include the Ioctls.h file to your source project.
   This method provides greater flexibility and control, but is also
   more complicated. Example source code of how to interface directly
   with the driver can be found in the MPUSBAPI source code.
