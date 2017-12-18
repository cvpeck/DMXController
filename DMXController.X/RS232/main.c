/*
 * File:   newmain.c
 * Author: peckc
 *
 * Created on March 18, 2017, 9:39 AM
 */


#include <xc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uart.h"
#include "main.h"
#include "crc8.h"
#include "debug.h"
#include "commands.h"

#define BAUDRATE 9600
#define TIMEOUT_DATA 2000 // time in ms before failure assumed
#define UART_RETRY_TIME 100 // time in ms before retrying UART for data


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define DATA_HEADER '\x55','\x11'
#define DATA_HEADER_LENGTH 2

#define COMMAND_LENGTH 1

#define DATA_LENGTH_HEADER 2

#define MAX_DATA_LENGTH 64

// CRC length in bytes
#define CRC_LENGTH 2



/* array and enum below must be in sync! */
int (* state[])(void) = { processHeader, processCommand, processDataLength, processData, processChecksum, sendStatus};

struct transition {   
            state_codes src_state;
            ret_codes   ret_code;
            state_codes dst_state;
};

struct diagnostics diagnostic_options;


//TODO create a struct packet containing all the unionss and vars below to represent the data packet structure



typedef union {
 unsigned char c[DATA_HEADER_LENGTH];
 unsigned int i;
} data_header_u;

typedef union {
    unsigned char c[COMMAND_LENGTH];
    unsigned int i;
} command_u;


typedef union {
 unsigned char c[DATA_LENGTH_HEADER];
 unsigned int i;
} data_length_u;

typedef union {
 unsigned char c[CRC_LENGTH];
 unsigned int i;
} crc_u;



//data_length_u.ui = 0xAB;
// can also access c[0] c[1]]




typedef struct data_packet_type {
    data_header_u data_header;
    command_u command;
    data_length_u data_length;
    unsigned char data[MAX_DATA_LENGTH];
    crc_u crc;
};

struct data_packet_type data_packet;

/* transitions from end state aren't needed */
struct transition state_transitions[] = {
    {waitForHeader,     ok,     waitForCommand},
    {waitForHeader,     fail,   sendStatus},
    {waitForCommand,    ok,     waitForDataLength},
    {waitForCommand,    fail,   sendStatus},
    {waitForDataLength, ok,     waitForData},
    {waitForDataLength, fail,   sendStatus},
    {waitForData,       repeat, waitForData},
    {waitForData,       ok,     waitForChecksum},
    {waitForData,       fail,   sendStatus},
    {waitForChecksum,     ok,     sendStatus},
    {waitForChecksum,     fail,   sendStatus},
};

#define EXIT_STATE sendStatus
#define ENTRY_STATE waitForHeader

bool data_ready()
{
    unsigned int attempts=0;
    while (!UART_Data_Ready()) {
        //TODO refactor this so only one return
        __delay_ms(UART_RETRY_TIME);
        if(++attempts*UART_RETRY_TIME > TIMEOUT_DATA) return false;
    }
    return true;
}

int processHeader() {
    int retval=fail;
    printf("processHeader\n");
    if (data_ready())
    {

        UART_Read_Text(data_packet.data_header.c,DATA_HEADER_LENGTH);
        if (diagnostic_options.all_echo || diagnostic_options.echo_header) {
            UART_Write_NText(data_packet.data_header.c,DATA_HEADER_LENGTH);
        }
        //TODO - no idea what this is doing - needs redoing
        //return (strncmp(data,data_header,DATA_HEADER_LENGTH) ? fail : ok);
        retval=ok;
    }
    return retval;
}

int processCommand() {
    int retval=fail;
    printf("processCommand");
    if (data_ready())
    {
        UART_Read_Text(data_packet.command.c,COMMAND_LENGTH);
        if (diagnostic_options.all_echo || diagnostic_options.echo_command) {
            UART_Write_NText(data_packet.command.c,COMMAND_LENGTH);
        }

        switch (data_packet.command.i) {
            case CMD__DIAGNOSTICS_ALL_ECHO :
                diagnostic_options.all_echo = true;
                retval=ok;
                break;
            case CMD__DIAGNOSTICS_HEADER_ECHO :
                diagnostic_options.echo_header = true;
                retval=ok;
                break;
            case CMD__DIAGNOSTICS_COMMAND_ECHO :
                diagnostic_options.echo_command = true;
                retval=ok;
                break;
            default :
                retval=fail; 
        }
    }
    return retval;
}

int processDataLength() {
    int retval=fail;
    printf("processDataLength");
    if (data_ready())
    {
        UART_Read_Text(data_packet.data_header.c,DATA_LENGTH_HEADER);
        if (diagnostic_options.all_echo || diagnostic_options.echo_data_length) {
            UART_Write_NText(data_packet.data_header.c,DATA_LENGTH_HEADER);
        }
        //TODO check this comparison is the correct way around
        retval=((data_packet.data_header.i < MAX_DATA_LENGTH) ? fail : ok);
    }
    return retval;
}

int processData() {
    printf("processDataLength");
    int retval=fail;
    if (data_ready())
    {
        UART_Read_Text(data_packet.data,data_packet.data_length.i);
        if (diagnostic_options.all_echo || diagnostic_options.echo_data) {
            UART_Write_NText(data_packet.data, data_packet.data_length.i);
        }
        // No way of easily checking how much data read.
        retval = ok;
    }
    return retval;;    
}

int processChecksum() {
    int retval=fail;
    unsigned int x=0;
    printf("processChecksum");
    if (data_ready())
    {
        UART_Read_Text(data_packet.crc.c, CRC_LENGTH);
        if (diagnostic_options.all_echo || diagnostic_options.echo_data) {
            UART_Write_NText(data_packet.crc.c, CRC_LENGTH);
        }
        unsigned int crc_calculated = 0;
        for (x=0; x<DATA_HEADER_LENGTH; x++) {
            crc_calculated = crc8_update(data_packet.data_header.c[x]);
        }
        for (x=0; x<COMMAND_LENGTH; x++) {
            crc_calculated = crc8_update(data_packet.command.c[x]);
        }
        for (x=0; x<DATA_LENGTH_HEADER; x++) {
            crc_calculated = crc8_update(data_packet.data_length.c[x]);
        }
        for (x=0; x<data_packet.data_length.i; x++) {
            crc_calculated = crc8_update(data_packet.data[x]);
        }
        if (crc_calculated = data_packet.crc.i) retval=ok;
    }
    return retval;
}

int returnStatus() {
    return false;
}

state_codes lookup_transition(state_codes sc, ret_codes rc) {
    for (unsigned int i=0; i<sizeof(state_transitions); i++)
    {
        if((state_transitions[i].src_state==sc) && (state_transitions[i].ret_code==rc)) return state_transitions[i].dst_state;
    }
    return sc;
}

int main(int argc, char *argv[]) {
    state_codes cur_state = ENTRY_STATE;
    ret_codes rc;
    int (* state_fun)(void);


    
    initialise_diagnostics(&diagnostic_options);
    
    init_debug_uart();
    
    printf("Hello World\n");

    if (!UART_Init(BAUDRATE)) {
        // UART init returned 0 so failed
        printf("UART initialisation failed");
        return EXIT_FAILURE;
    }
    
    
            
    for (;;) {
        state_fun = state[cur_state];
        rc = state_fun();
        if (EXIT_STATE == cur_state)
            break;
        cur_state = lookup_transition(cur_state, rc);
    }

    return EXIT_SUCCESS;
}

