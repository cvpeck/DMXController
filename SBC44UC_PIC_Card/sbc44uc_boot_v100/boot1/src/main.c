/*********************************************************************
 *
 *                Microchip USB Bootloader Version 1.0
 *
 *********************************************************************
 * FileName:        main.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rawin Rojvanit       11/19/04     Original.
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"                        // Required
#include "system\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "system\usb\usb_compile_time_validation.h" // Optional

/** V A R I A B L E S ********************************************************/
#pragma udata

/** P R I V A T E  P R O T O T Y P E S ***************************************/

/** V E C T O R  R E M A P P I N G *******************************************/
#pragma code _HIGH_INTERRUPT_VECTOR = 0x000008
void _high_ISR (void)
{
    _asm goto RM_HIGH_INTERRUPT_VECTOR _endasm
}

#pragma code _LOW_INTERRUPT_VECTOR = 0x000018
void _low_ISR (void)
{
    _asm goto RM_LOW_INTERRUPT_VECTOR _endasm
}
#pragma code

/** Configuration Bits *******************************************/
#pragma config PLLDIV = 5           //Configure for 20Mhz crystal, 20/5 = 4Mhz (required for USB)
#pragma config CPUDIV = OSC1_PLL2   //Use 48MHz clock for system clock = 96MHz/2 = 48MHz
#pragma config USBDIV = 2           //USB Clock source from the 96 MHz PLL divided by 2
#pragma config FOSC = HSPLL_HS      //Oscillator Selection bits - HS oscillator, PLL enabled, HS used by USB
#pragma config FCMEN = OFF          //Fail-Safe Clock Monitor Enable bit: Disable
#pragma config IESO = OFF           //Internal/External Oscillator Switchover bit: Disable
#pragma config PWRT = ON            //Power Up timer enabled
#pragma config BOR = SOFT           //Brown-out Reset enabled and controlled by software (SBOREN is enabled)
#pragma config BORV = 2             //Brown-out voltage = 2.7V
#pragma config VREGEN = ON          //USB Voltage regulator enabled
#pragma config WDT = OFF            //Watchdog timer HW Disabled - SW Controlled
#pragma config WDTPS = 128          //Watchdog timer = 512ms (128 x 4ms)
#pragma config MCLRE = ON           //Master clear enabled
#pragma config LPT1OSC = OFF        //Disabled
#pragma config PBADEN = OFF         //Port B0-4 are digital inputs on reset
#pragma config CCP2MX = ON          //RC1
#pragma config STVREN = ON          //Stack overflow reset enable
#pragma config LVP = OFF            //Low voltage programming disabled
#pragma config ICPRT = OFF          //Dedicated In-Circuit Debug/Programming Port (ICPORT) disabled
#pragma config XINST = OFF          //Extended mode off
#pragma config DEBUG = OFF          //Debug mode off
#pragma config CP0 = OFF 
#pragma config CP1 = OFF 
#pragma config CP2 = OFF 
#pragma config CP3 = OFF 
#pragma config CPB = OFF 
#pragma config CPD = OFF 
#pragma config WRT0 = OFF 
#pragma config WRT1 = OFF 
#pragma config WRT2 = OFF 
#pragma config WRT3 = OFF 
#pragma config WRTB = OFF 
#pragma config WRTC = OFF 
#pragma config WRTD = OFF 
#pragma config EBTR0 = OFF 
#pragma config EBTR1 = OFF 
#pragma config EBTR2 = OFF 
#pragma config EBTR3 = OFF 
#pragma config EBTRB = OFF



/** D E C L A R A T I O N S **************************************************/
#pragma code
/******************************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *****************************************************************************/
void main(void)
{
    byte temp;
    temp = ADCON1;
    ADCON1 |= 0x0F;

    RCONbits.SBOREN = 1;    //Enable brown out reset    
    
    /////////////////////////////////////////////////
    //Check for bootloader mode entry condition. To enter this mode, B6 and B7 have to be
    //shorted together.
    //Set B6 as an input, write 0 and 1 out on B7 and confirm B6 has the same value = they are connected!
    //Set B6 as input so we can write out to B7 (discharge it)
    temp = 0;
    TRISBbits.TRISB6 = 1;   //B6 is an input. It has pull-down resistors
    LATBbits.LATB7 = 1;
    TRISBbits.TRISB7 = 0;   //B7 is an output
    //Are B6 and B7 connected?
    if (PORTBbits.RB6 == 1) {
        LATBbits.LATB7 = 0;      //Now put 1 on pin B7
        //Are B6 and B7 connected?
        if (PORTBbits.RB6 == 0) {
            //We have entered bootloader mode.
            TRISBbits.TRISB7 = 1;  //Reset B7 is an input. This must be done fast in case user has something on B7!
            temp = 1;   //Entered bootloader mode
        }
    }
    TRISBbits.TRISB7 = 1;  //Reset B7 is an input
    //We have NOT entered bootloader mode, return to main application
    if (temp == 0) {
        ADCON1 = temp;          // Restore reset value
        _asm goto RM_RESET_VECTOR _endasm
    }
    
    //Bootload Mode
    mInitAllLEDs();
    mInitializeUSBDriver();     // See usbdrv.h
    USBCheckBusStatus();        // Modified to always enable USB module
    while(1)
    {
        USBDriverService();     // See usbdrv.c
        BootService();          // See boot.c
    }//end while
}//end main

#pragma code user = RM_RESET_VECTOR

/** EOF main.c ***************************************************************/
