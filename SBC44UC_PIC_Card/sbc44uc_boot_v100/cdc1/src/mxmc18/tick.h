/**
 * @brief           Tick Functions
 * @file            tick.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 compiler
 *
 * @section description Description
 **********************************
 *  To use this module:
 *  - Customize defines specified in "tick Configuration" below
 *  - tickInit() must be called before using this module
 *  - tickService() must be called every system tick time (As defined by TICKS_PER_SECOND)
 *
 *
 * @subsection tick_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 // *********************************************************************
 // -------------- Tick Configuration --------------------
 // *********************************************************************
 //The following should be defined in the project's projdefs.h file OR on the command line
 #define TICKS_PER_SECOND 1000   //Define number of ticks per second for this project
 #define TICK_HAS_8BIT_VAR       //Define this is project should have a tick8 8 bit tick variable
 #define TICK_HAS_8BITX10_VAR    //Define this is project should have a tick8x10 8 bit tick variable
 @endcode
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
 * 2004-12-17, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#ifndef _TICK_H_
#define _TICK_H_

/////////////////////////////////////////////////
//External variables
extern uint16 tick16;

#ifdef TICK_HAS_8BIT_VAR
extern uint8 tick8;
#endif

#ifdef TICK_HAS_8BITX10_VAR
extern uint8 tick8x10;
extern uint8 tickTmr10;
#endif


/**
 * Gets difference between given value and "16 bit tick"
 */
#define tick16GetDiff(a)       ((uint16)(tick16 - a))

/**
 * Will return true once the "16 bit tick" is > given value
 * DEPRECATED!! Use tick16GetDiff() in stead!!
 */
#define tick16GreaterThan(a)      ((a - tick16) & 0x80)

/**
 * Gets difference between given value and "8 bit tick"
 */
#define tick8GetDiff(a)       ((uint8)(tick8 - a))

/**
 * Will return true once the "8 bit tick" is > given value
 * DEPRECATED!! Use tick8GetDiff() in stead!!
 */
#define tick8GreaterThan(a)      ((a - tick8) & 0x80)

/**
 * Gets difference between given value and "8 bit times 10 tick"
 */
#define tick8x10GetDiff(a)      ((uint8)(tick8x10 - a))

/**
 * Will return true once the "8 bit times 10 tick" is > given value
 * DEPRECATED!! Use tick8x10GetDiff() in stead!!
 */
#define tick8x10GreaterThan(a)      ((a - tick8x10) & 0x80)

/**
 * Initializes tick values
 */
void tickInit(void);

#ifdef TICK_HAS_8BIT_VAR
    #define INC_TICK8() (tick8++)
#else
    #define INC_TICK8()
#endif

#ifdef TICK_HAS_8BITX10_VAR
    #define INC_TICK8X10() if (--tickTmr10 == 0) {tickTmr10 = 10; tick8x10++;}
#else
    #define INC_TICK8X10()
#endif

/**
 * Increments all tick values when called
 */
#define tickService() {                 \
                                        \
    /* Increment tick8 if used */       \
    INC_TICK8();                        \
                                        \
    /* Increment tick8x10 if used */    \
    INC_TICK8X10();                     \
                                        \
    tick16++;                           \
}
#endif
