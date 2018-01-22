/*********************************************************************
 *
 *                Microchip USB C18 Firmware Version 1.0
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
 * Rawin Rojvanit       11/19/04    Original.
 ********************************************************************/


/////////////////////////////////////////////////
// Includes
#include <p18cxxx.h>
#include "system\typedefs.h"                        // Required
#include "system\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "system\usb\usb_compile_time_validation.h" // Optional
#include "user\user.h"                              // Modifiable

#include "projdefs.h"
#include "mxmc18\tick.h"                            // Modtronix library


/////////////////////////////////////////////////
// Varaiables
#pragma udata


/////////////////////////////////////////////////
// Prototypes
static void InitializeSystem(void);
void USBTasks(void);
void high_isr(void);
void low_isr(void);

/////////////////////////////////////////////////
// Vector Remapping required for bootloader. Bootloader is from 0 - 0x7ff
extern void _startup (void);        // See c018i.c in your C18 compiler dir
#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code

#pragma code _HIGH_INTERRUPT_VECTOR = 0x000808
void _high_ISR (void)
{
    _asm goto high_isr _endasm
}
#pragma code

#pragma code _LOW_INTERRUPT_VECTOR = 0x000818
void _low_ISR (void)
{
    _asm goto low_isr _endasm
}
#pragma code

/////////////////////////////////////////////////
//Low Interrupt ISR
#pragma interrupt low_isr
void low_isr(void) {
}
#pragma code

/////////////////////////////////////////////////
//High Interrupt ISR
#pragma interrupt high_isr
void high_isr(void) {
    //static word isrTmp = 0;
    
    //TMR2 is used for the ticks
    if (PIR1bits.TMR2IF)
    {   
        PIR1bits.TMR2IF = 0;
        tickService();

        // TEST TEST
        //if (isrTmp == 0) {
        //    isrTmp = 500;
        //    mLED_1 = !mLED_1;   // TEST TEST
        //}
        //isrTmp--;
    }
}
#pragma code

/**
 * Main function
 * Main program entry point.
 *
 */
void main(void)
{
    InitializeSystem();

    //Initialize tick module
    tickInit();

    while(1)
    {
        USBTasks();         // USB Tasks
        ProcessIO();        // See user\user.c & .h

    }//end while
}

/**
 * InitializeSystem is a centralize initialization routine.
 * All required USB initialization routines are called from here.
 *
 * User application initialization routine should also be
 * called from here.
 *
 */
static void InitializeSystem(void)
{
    ADCON1 |= 0x0F;                 // Default all pins to digital
    

    /////////////////////////////////////////////////
    //Setup timer2 as a 1ms timer
    /*
    //Prescaler of 16 at 48MHz = 48,000,000 / 16 = 3,000,000 hz
    //Postcaler of 12 at 48MHz = 3,000,000 / 12 = 250,000
    //Set PR2 = 249. This causes timer to reset when it reaches 249 = 250,000 / 250 = 1000hz = 1ms
    T2CON = 0x5e;           //xxxx xx10 - Prescaler = 16
                            //xxxx x1xx - Tmr2 on
                            //x101 1xxx - Postscaler = 11 (0=1, 1=2 ... 11=12)
    PR2 = 249;
    */

    //Prescaler of 4 at 12MHz = 12,000,000 / 16 = 3,000,000 hz
    //Postcaler of 12 at 12MHz = 3,000,000 / 12 = 250,000
    //Set PR2 = 249. This causes timer to reset when it reaches 249 = 250,000 / 250 = 1000hz = 1ms
    T2CON = 0x5d;           //xxxx xx01 - Prescaler = 4
                            //xxxx x1xx - Tmr2 on
                            //x101 1xxx - Postscaler = 11 (0=1, 1=2 ... 11=12)
    //PR2 = 249;
    PR2 = 249;
    PIE1bits.TMR2IE = 1;    //Enalbe Timer2 interrupt


    /////////////////////////////////////////////////
    //USB stack defines
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See io_cfg.h
    #endif
    
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;
    #endif
    
    mInitializeUSBDriver();         // See usbdrv.h
    
    UserInit();                     // See user.c & .h


    /////////////////////////////////////////////////
    //Global interrupt enable
    INTCONbits.PEIE = 1;            //Enable Peripheral interrups (TMR2, ....)
    INTCONbits.GIE = 1;             //Global interrupt enable
}

/**
 * Service loop for USB tasks.
 *
 * @PreCondition    InitializeSystem has been called.
 */
void USBTasks(void)
{
    /*
     * Servicing Hardware
     */
    USBCheckBusStatus();                    // Must use polling method
    if(UCFGbits.UTEYE!=1)
        USBDriverService();                 // Interrupt or polling method
    
    #if defined(USB_USE_CDC)
    CDCTxService();
    #endif

}

