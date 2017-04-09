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

#include "uart.h"
#include "main.h"
#include "debug.h"

#define BAUDRATE 9600


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define DATA_HEADER '\x55','\x11'
#define DATA_HEADER_LENGTH 2



/* array and enum below must be in sync! */
int (* state[])(void) = { processHeader, processCommand, processDataLength, processData, processChecksum, returnStatus};

struct transition {   
            state_codes src_state;
            ret_codes   ret_code;
            state_codes dst_state;
};


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
    char data[DATA_HEADER_LENGTH];
    char data_header[DATA_HEADER_LENGTH] = {DATA_HEADER};
    
    printf("processHeader\n");
    while (!UART_Data_Ready()) { // TODO add timeout here
        __delay_ms(100);
    }
    UART_Read_Text(data,DATA_HEADER_LENGTH);
    return (strncmp(data,data_header,DATA_HEADER_LENGTH) ? fail : ok);
}

int processCommand() {
    return false;
}

int processDataLength() {
    return false;
}

int processData() {
    return false;
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
    
    init_debug_uart();
    
    printf("Hello World\n");

    if (!UART_Init(BAUDRATE)) {
        // UART init returned 0 so failed
        printf("UART initialisation failed");
        return EXIT_FAILURE;
    }
    
    unsigned char command;
    
            
    for (;;) {
        state_fun = state[cur_state];
        rc = state_fun();
        if (EXIT_STATE == cur_state)
            break;
        cur_state = lookup_transition(cur_state, rc);
    }

    return EXIT_SUCCESS;
}

