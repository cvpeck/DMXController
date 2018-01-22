 /**
 * @brief           Example User Program to be used with SBC44UC Bootloader
 * @file            main.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 2.30.01 or higher <br>
 *
 *
 *********************************************************************
 *            Description
 *********************************************************************
 * This project contains a very simple example program for flashing the
 * LED on port RB6 of the Modtronix SBC44UC single board computer. It
 * is meant to be used with the bootloader on the board.
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2007-06-16, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


/////////////////////////////////////////////////
// Includes
#include <p18cxxx.h>


/////////////////////////////////////////////////
// Defines
#pragma udata
#define mLED_1              LATBbits.LATB6
typedef unsigned char   byte;           // 8-bit
typedef unsigned int    word;           // 16-bit
typedef unsigned long   dword;          // 32-bit


/////////////////////////////////////////////////
// Varaiables
#pragma udata


/////////////////////////////////////////////////
// Prototypes


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
    ;
}

#pragma code _LOW_INTERRUPT_VECTOR = 0x000818
void _low_ISR (void)
{
    ;
}
#pragma code


/**
 * Main function
 *
 */
void main(void)
{
    dword cnt=0;
    ADCON1 |= 0x0F;                 // Default all pins to digital

    //Initialize B6
    LATB &= 0x40;
    TRISB &= ~0x40;

    while(1)
    {
        //Toggle LED on port B6
        if (cnt-- == 0) {
            cnt = 0x1ffff;
            mLED_1 = !mLED_1;
        }
    }//end while
}//end main

