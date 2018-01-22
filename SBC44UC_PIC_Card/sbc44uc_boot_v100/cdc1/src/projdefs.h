/**
 * @brief           Project configuration
 * @file            projdefs.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 compiler
 * @ingroup         mod_conf_projdefs
 *
 * Description
 **********************************
 *
 * This file is used to configure the Modtronix libraries located
 * in the "mxmc18" folder.
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
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

#ifndef _PROJDEFS_H_
#define _PROJDEFS_H_

//Used by Modtronix Library
typedef unsigned char   uint8;          // 8-bit
typedef unsigned int    uint16;         // 16-bit
typedef unsigned long   uint32;         // 32-bit

/*******************************************************
-------------- tick Configuration --------------------
********************************************************/
#define TICKS_PER_SECOND 1000   //Define number of ticks per second for this project
#define TICK_HAS_8BIT_VAR       //Define this is project should have a tick8 8 bit tick variable
//#define TICK_HAS_8BITX10_VAR    //Define this is project should have a tick8x10 8 bit tick variable
/********************************************************/

#endif  //_PROJDEFS_H_
