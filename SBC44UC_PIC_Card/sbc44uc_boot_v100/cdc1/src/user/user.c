/*********************************************************************
 *
 *                Microchip USB C18 Firmware Version 1.0
 *
 *********************************************************************
 * FileName:        user.c
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

/******************************************************************************
 * CDC RS-232 Emulation Tutorial Instructions:
 ******************************************************************************
 * Refer to Application Note AN956 for explanation of the CDC class.
 * 
 * First take a look at Exercise_Example() and study how functions are called.
 * 
 * There are five exercises, each one has a solution in the CDC\user\solutions.
 * Scroll down and look for Exercise_01,_02,_03,_04, and _05.
 * Instructions on what to do is inside each function.
 * 
 *****************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include "system\typedefs.h"

#include "system\usb\usb.h"

#include "io_cfg.h"             // I/O pin mapping
#include "user\user.h"
#include "user\temperature.h"

#include "projdefs.h"
#include "mxmc18\tick.h"                            // Modtronix library


/** V A R I A B L E S ********************************************************/
#pragma udata
byte old_sw2,old_sw3;

char input_buffer[64];
char output_buffer[64];

rom char welcome[]={"PIC18F4550 Full-Speed USB - CDC RS-232 Emulation Demo\r\n\r\n"};
rom char ansi_clrscr[]={"\x1b[2J"};         // ANSI Clear Screen Command

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void InitializeUSART(void);
void BlinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);

byte Exercise_Example(void);

//Modtronix Examples
void Example1(void);
void Example2(void);
void Example3(void);

void Exercise_01(void);
void Exercise_02(void);
void Exercise_03(void);
void Exercise_04(void);
void Exercise_05(void);

/** D E C L A R A T I O N S **************************************************/
#pragma code
void UserInit(void)
{
    mInitAllLEDs();
    mInitAllSwitches();
    old_sw2 = sw2;
    old_sw3 = sw3;
    
    InitTempSensor();
    
    InitializeUSART();   

}//end UserInit

void InitializeUSART(void)
{
    TRISCbits.TRISC7=1; // RX
    TRISCbits.TRISC6=0; // TX
    SPBRG = 0x71;
    SPBRGH = 0x02;      // 0x0271 for 48MHz -> 19200 baud
    TXSTA = 0x24;       // TX enable BRGH=1
    RCSTA = 0x90;       // continuous RX
    BAUDCON = 0x08;     // BRG16 = 1
}//end InitializeUSART

/******************************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user routines.
 *                  It is a mixture of both USB and non-USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void ProcessIO(void)
{   
    static byte init=0;

    BlinkUSBStatus();
    // User Application USB tasks
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;

    //Print welcome message once in beginning
    if (Exercise_Example() == 1) {
        //Select which exercise to use.
        #if defined(EXAMPLE1)
            Example1();  //Hello World Example - prints hello world once a second
        #elif defined(EXAMPLE2)
            Example2();  //Echo eample. Echos "Received: x" showing the character you typed
        #endif
    }

}//end ProcessIO

/**
 * Modtronix Example 1 for the SBC44UC board.
 * This example writes "Hello World!" to the virtual serial port every second.
 */
void Example1(void)
{
    static uint16 tmr1sec = 0;
    
    /////////////////////////////////////////////////
    //Enter every 60ms
    if (tick16GetDiff(tmr1sec) > 1000) {
        tmr1sec += 1000;

        if(mUSBUSARTIsTxTrfReady())
        {
            putrsUSBUSART("\rHello World\r\n");
        }
    }
}

/**
 * Modtronix Example 2 for the SBC44UC board.
 */
void Example2(void)
{
    static byte bytesRead;
    
    static byte smEx2State = 0;
    // State Machine states
    typedef enum _SM_CMD
    {
        EX2_WAIT_FOR_INPUT,     // Wait for input from user via vircual serial port
        EX2_WAIT_TX_READY       // Wait for driver ready to accept a new string to write to the USB?
    } SM_CMD;
    
    switch (smEx2State) {
        case EX2_WAIT_FOR_INPUT:
            //Check if any data was received via the virtual serial port
            if(bytesRead = getsUSBUSART(input_buffer, 8)) {
                smEx2State = EX2_WAIT_TX_READY;
            }
        break;
        case EX2_WAIT_TX_READY:
            //Write "Received: ", followed by data received, to virtual serial port
            if(mUSBUSARTIsTxTrfReady())
            {
                output_buffer[0] = '\r';
                output_buffer[1] = 'R';
                output_buffer[2] = 'e';
                output_buffer[3] = 'c';
                output_buffer[4] = 'e';
                output_buffer[5] = 'i';
                output_buffer[6] = 'v';
                output_buffer[7] = 'e';
                output_buffer[8] = 'd';
                output_buffer[9] = ':';
                output_buffer[10] = ' ';
                output_buffer[11] = input_buffer[0];
                output_buffer[12] = '\r';
                output_buffer[13] = '\n';
                output_buffer[14] = '\0';
                putsUSBUSART(output_buffer);

                //Back to wait for input state
                smEx2State = EX2_WAIT_FOR_INPUT;
            }
        break;
    }
}

byte Exercise_Example(void)
{
    static byte start_up_state = 0;
    
    if(start_up_state == 0)
    {
        if(mUSBUSARTIsTxTrfReady())
        {
            putrsUSBUSART(ansi_clrscr);
            start_up_state++;
        }
    }
    else if(start_up_state == 1)
    {
        if(mUSBUSARTIsTxTrfReady())
        {
            putrsUSBUSART("\rMicrochip Technology Inc., 2004\r\n");
            start_up_state++;
        }
    }
    else if(start_up_state == 2)
    {
        if(mUSBUSARTIsTxTrfReady())
        {
            putrsUSBUSART(welcome);
            start_up_state++;
        }
    }

    if (start_up_state == 3) return 1;

    return 0;   //Not finished
    
}//end Exercise_Example


/**
 * This is an original Microchip Example function. Is not used for the Modtronix SBC44UC board!
 */
void Exercise_01(void)
{
    /*
     * Write code in this function that sends a literal null-terminated
     * string of text ("Hello World!\r\n") to the PC when switch 2 is
     * pressed.
     *
     * Useful functions:
     *  Switch2IsPressed() returns '1' when switch 2 is pressed.
     *  putrsUSBUSART(...);
     *
     * See examples in Exercise_Example();
     *
     * Remember, you must check if cdc_trf_state is ready for another
     * transfer or not. When it is ready, the value will equal CDC_TX_READY,
     * or use macro: mUSBUSARTIsTxTrfReady()
     */
     
    /* Insert code here - 3 lines */

    /* End */
    
}//end Exercise_01

rom char ex02_string[]={"Type in a string here.\r\n"};
/**
 * This is an original Microchip Example function. Is not used for the Modtronix SBC44UC board!
 */
void Exercise_02(void)
{
    /*
     * Write code in this function that sends a null-terminated string
     * of text stored in program memory pointed to by "ex02_string" to
     * the PC when switch 3 is pressed.
     *
     * ex02_string is declared right above this function.
     *
     * Useful functions:
     *  Switch3IsPressed() returns '1' when switch 3 is pressed.
     *  putrsUSBUSART(...);
     *
     * See examples in Exercise_Example();
     *
     * Remember, you must check if cdc_trf_state is ready for another
     * transfer or not. When it is ready, the value will equal CDC_TX_READY,
     * or use macro: mUSBUSARTIsTxTrfReady()
     */

    /* Insert code here - 3 lines*/

    /* End */
    
}//end Exercise_02

/**
 * This is an original Microchip Example function. Is not used for the Modtronix SBC44UC board!
 */
void Exercise_03(void)
{
    /*
     * Write code in this function that reads data from USB and
     * toggles LED D4 when the data read equals ASCII character '1' (0x31)
     *
     * Useful functions:
     *  byte getsUSBUSART(char *buffer, byte len)   See cdc.c for details
     *  mLED_4_Toggle();
     *
     * Use input_buffer[] to store data read from USB.
     */

    /* Insert code here - 3 lines */

    /* End */
    
}//end Exercise_03

/**
 * This is an original Microchip Example function. Is not used for the Modtronix SBC44UC board!
 */
void Exercise_04(void)
{
    /*
     * Before starting Exercise_04(), comment out the call to Exercise_01()
     * in ProcessIO(); This function will need to check Switch2IsPressed().
     *
     * Write code in this function that sends the following 4 bytes of
     * data: 0x30,0x31,0x32,0x33 when switch 2 is pressed.
     * Note that these data is not null-terminated and is located in
     * the data memory.
     *
     * Useful functions:
     *  Switch2IsPressed() returns '1' when switch 2 is pressed.
     *  mUSBUSARTTxRam(byte *pData, byte len) See cdc.h for details.
     *
     * Use output_buffer[] to store the four bytes data.
     *
     * Remember, you must check if cdc_trf_state is ready for another
     * transfer or not. When it is ready, the value will equal CDC_TX_READY,
     * or use macro: mUSBUSARTIsTxTrfReady()
     */
    
    /* Insert code here - 7 lines */

    /* End */
	
}//end Exercise_04

/**
 * This is an original Microchip Example function. Is not used for the Modtronix SBC44UC board!
 */
void Exercise_05(void)
{
    /*
     * The PICDEM Full-Speed USB Demo Board is equipped with a 
     * temperature sensor. See temperature.c & .h for details.
     *
     * All necessary functions to collect temperature data have
     * been called. This function updates the data a few times
     * every second. The program currently sends out the
     * temperature data to the PC via UART.
     *
     * You can check this by hooking up a serial cable and
     * set your serial port to 19200 baud, 8 bit data, 1 Stop,
     * no parity.
     *
     * The program assumes CPU Frequency = 48 MHz to generate
     * the correct SPBRG value for 19200 baud transmission.
     *
     * Modify the code to send the ASCII string stored in
     * tempString to the PC via USB instead of UART.
     *
     * The temperature data is stored in tempString array in
     * ASCII format and is null-terminated.
     *
     * Useful function:
     *  putsUSBUSART(...);
     *
     *  Note: It is 'puts' and not 'putrs' to be used here.
     *
     * Remember, you must check if cdc_trf_state is ready for another
     * transfer or not. When it is ready, the value will equal CDC_TX_READY,
     * or use macro: mUSBUSARTIsTxTrfReady()
     */
    
    static word ex05_count;
    
    if(ex05_count == 0)
    {
        AcquireTemperature();               // Read temperature from sensor
        UpdateCelsiusASCII();               // Convert to ASCII, stored in
                                            // "tempString", See temperature.c

        /* Modify the code below - 3 lines */

            putsUSART(tempString);
            ex05_count = 10000;

        /* End */
    }
    else
        ex05_count--;
    
}//end Exercise_05

/******************************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus blinks the LED from 1 to 8 times corresponding
 *                  to the USB device state.
 *
 * Note:            mLED macros can be found in io_cfg.h
 *                  usb_device_state is declared in usbmmap.c and is modified
 *                  in usbdrv.c, usbctrltrf.c, and usb9.c
 *****************************************************************************/
void BlinkUSBStatus(void)
{
    static word led_blink_cnt=0;    //Current blink st
    static byte led_blinks=0;       //Indicate how many blinks that LED should have

    static uint8 tmrLed;
    
    /////////////////////////////////////////////////
    //Enter every 60ms
    if (tick8GetDiff(tmrLed) > 60) {
        tmrLed += 60;
        if (led_blink_cnt == 0) {
            led_blink_cnt = 32;
        }
        led_blink_cnt--;
    }

    //Check if LED must be on
    if ((led_blink_cnt&0x03)==0x03) {
        if ((led_blinks*4) > led_blink_cnt)
            mLED_1 = 1;
        else
            mLED_1 = 0;
    }
    else {
        mLED_1 = 0;
    }

    //Each USB state will blink the LED different number of times
    if(UCONbits.SUSPND == 1)
    {
        led_blinks=1;
    }
    else
    {
        if(usb_device_state == DETACHED_STATE)
        {
            led_blinks=2;
        }
        else if(usb_device_state == ATTACHED_STATE)
        {
            led_blinks=3;
        }
        else if(usb_device_state == POWERED_STATE)
        {
            led_blinks=4;
        }
        else if(usb_device_state == DEFAULT_STATE)
        {
            led_blinks=5;
        }
        else if(usb_device_state == ADDRESS_STATE)
        {
            led_blinks=6;
        }
        else if(usb_device_state == CONFIGURED_STATE)
        {
            led_blinks=7;
        }//end if(...)
    }//end if(UCONbits.SUSPND...)

}//end BlinkUSBStatus

BOOL Switch2IsPressed(void)
{
    if(sw2 != old_sw2)
    {
        old_sw2 = sw2;                  // Save new value
        if(sw2 == 0)                    // If pressed
            return TRUE;                // Was pressed
    }//end if
    return FALSE;                       // Was not pressed
}//end Switch2IsPressed

BOOL Switch3IsPressed(void)
{
    if(sw3 != old_sw3)
    {
        old_sw3 = sw3;                  // Save new value
        if(sw3 == 0)                    // If pressed
            return TRUE;                // Was pressed
    }//end if
    return FALSE;                       // Was not pressed
}//end Switch3IsPressed

/** EOF user.c ***************************************************************/
