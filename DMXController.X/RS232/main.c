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

#define MAX_DATA_LENGTH 1024



/* array and enum below must be in sync! */
int (* state[])(void) = { processHeader, processCommand, processDataLength, processData, processChecksum, returnStatus};

struct transition {   
            state_codes src_state;
            ret_codes   ret_code;
            state_codes dst_state;
};

struct diagnostics diagnostic_options;

unsigned int command;

union {
 unsigned char c[DATA_HEADER_LENGTH];
 unsigned int i;
} data_header_length_u;

union {
    unsigned char c[COMMAND_LENGTH];
    unsigned int i;
} command_u;


union {
 unsigned char c[DATA_LENGTH_HEADER];
 unsigned int i;
} data_length_header_u;

//data_length_u.ui = 0xAB;
// can also access c[0] c[1]]

int data_length;

char data[MAX_DATA_LENGTH];

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

int processHeader() {
    unsigned int attempts=0;
    printf("processHeader\n");
    
    while (!UART_Data_Ready()) { 
        __delay_ms(UART_RETRY_TIME);
        if(++attempts*UART_RETRY_TIME > TIMEOUT_DATA) return fail;
    }
    UART_Read_Text(data_header_length_u.c,DATA_HEADER_LENGTH);
    if (diagnostic_options.all_echo || diagnostic_options.echo_header) {
        UART_Write_NText(data_header_length_u.c,DATA_HEADER_LENGTH);
    }
    //TODO - no idea what this is doing - needs redoing
    //return (strncmp(data,data_header,DATA_HEADER_LENGTH) ? fail : ok);
    return ok;
}

int processCommand() {
    unsigned int attempts=0;

    while (!UART_Data_Ready()) { 
        __delay_ms(UART_RETRY_TIME);
        if(++attempts*UART_RETRY_TIME > TIMEOUT_DATA) return fail;
    }
    printf("processCommand");
    UART_Read_Text(command_u.c,COMMAND_LENGTH);
    if (diagnostic_options.all_echo || diagnostic_options.echo_command) {
        UART_Write_NText(command_u.c,COMMAND_LENGTH);
    }
    
    command=command_u.i;
    
    switch (command) {
        case CMD__DIAGNOSTICS_ALL_ECHO :
            diagnostic_options.all_echo = true;
            return ok;
            
        case CMD__DIAGNOSTICS_HEADER_ECHO :
            diagnostic_options.echo_header = true;
            return ok;
            
        case CMD__DIAGNOSTICS_COMMAND_ECHO :
            diagnostic_options.echo_command = true;
            return ok;
            
        default :
            return fail; 
    }
}

int processDataLength() {
    unsigned int attempts=0;

    while (!UART_Data_Ready()) { 
        __delay_ms(UART_RETRY_TIME);
        if(++attempts*UART_RETRY_TIME > TIMEOUT_DATA) return fail;
    }
    
    printf("processDataLength");
    UART_Read_Text(data_length_header_u.c,DATA_LENGTH_HEADER);
    if (diagnostic_options.all_echo || diagnostic_options.echo_data_length) {
        UART_Write_NText(data_length_header_u.c,DATA_LENGTH_HEADER);
    }
    data_length = data_length_header_u.i;
    //TODO check this comparison is the correct way around
    return ((data_length < MAX_DATA_LENGTH) ? fail : ok);
}

int processData() {
    unsigned int attempts=0;

    while (!UART_Data_Ready()) { 
        __delay_ms(UART_RETRY_TIME);
        if(++attempts*UART_RETRY_TIME > TIMEOUT_DATA) return fail;
    }
    
    printf("processDataLength");
    UART_Read_Text(data,data_length);
    if (diagnostic_options.all_echo || diagnostic_options.echo_data) {
        UART_Write_NText(data, data_length);
    }
    // TODO return fail if length of data read less than data_length
    // how to check length of data?
    return ((data < MAX_DATA_LENGTH) ? fail : ok);
}

int processChecksum() {
    return false;
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

