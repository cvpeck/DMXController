This folder contains the Microchip Bootloader code downloaded from www.microchip.com.
It has been modified to run on the SBC44UC. The following modifications were done:

1) Appended following line to "Include Path" of project properties in MPLAB project
   window (accessed via <project><build options...><project> menu):
   ;.;..;..\..;..\..\..;..\..\..\..
   For some reason, the project did not find many include files without this line.

2) When plugging the SBC44UC board to the PC for the first time, it might be neccessary
   to install USB drivers. If asked, specify following location for drivers:
   ..\Pc\MCHPUSB Driver\Release
