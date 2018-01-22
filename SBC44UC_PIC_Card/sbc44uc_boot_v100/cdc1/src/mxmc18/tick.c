/**
 * @brief           Tick Functions
 * @file            tick.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        BoostC compilers<br>
 *                  CCS PCM and PCH compilers<br>
 *                  HITECH PICC or PICC-18<br>
 *                  MPLAB C18 compiler
 *
 *********************************************************************
 * File History
 *
 * 2004-12-17, David Hosken (DH):
 *    - Initial version
 *********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is intended and supplied to you,
 * the Company customer, for use solely and exclusively on products
 * manufactured by Modtronix Engineering. The code may be modified
 * and can be used free of charge for non commercial and commercial
 * applications. All rights are reserved. Any use in violation of the
 * foregoing restrictions may subject the user to criminal sanctions
 * under applicable laws, as well as to civil liability for the
 * breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

#include "projdefs.h"


/////////////////////////////////////////////////
// Defines
#ifndef TICKS_PER_SECOND
#error "TICKS_PER_SECOND is not to be defined!"
#endif


/////////////////////////////////////////////////
// Global variables. Lower nibble is current state, upper nibble is latched values
#ifdef TICK_HAS_8BIT_VAR
uint8 tick8;     //Simple byte variable for tick
#endif

#ifdef TICK_HAS_8BITX10_VAR
uint8 tick8x10;     //Simple byte variable for tick, incremented every 10 ticks
uint8 tickTmr10;    //internal timer used to see when 10 ticks has passed
#endif

uint16 tick16;        //Default 16 bit tick


/////////////////////////////////////////////////
// Function prototypes

/**
 * Initializes tick values
 */
void tickInit() {
    #ifdef TICK_HAS_8BIT_VAR
    tick8 = 0;
    #endif

    #ifdef TICK_HAS_8BITX10_VAR
    tick8x10 = 0;
    tickTmr10 = 10;
    #endif

    tick16 = 0;
}
