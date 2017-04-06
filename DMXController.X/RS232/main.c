/*
 * File:   newmain.c
 * Author: peckc
 *
 * Created on March 18, 2017, 9:39 AM
 */


#include <xc.h>
#include <stdbool.h>

#include "uart.h"
#include "main.h"

#define BAUDRATE 9600


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1




/* array and enum below must be in sync! */
int (* state[])(void) = { processHeader, processCommand, processDataLength, processData, processChecksum, returnStatus};

// element definition is to allow iteration across struct
struct transition {
    union {
        struct trans {
            state_codes src_state;
            ret_codes   ret_code;
            state_codes dst_state;
        }trans ;
        unsigned int elements[3];
    }; 
};


/* transitions from end state aren't needed */
struct transition state_transitions[] = {
    {trans.src_state = waitForHeader,     trans.ret_code = ok,     trans.dst_state = waitForCommand},
    {trans.src_state = waitForHeader,     trans.ret_code = fail,   trans.dst_state = sendStatus}
};
    
//,
//    {waitForCommand,    ok,     waitForDataLength},
//    {waitForCommand,    fail,   sendStatus},
//    {waitForDataLength, ok,     waitForData},
//    {waitForDataLength, fail,   sendStatus},
//    {waitForData,       repeat, waitForData},
//    {waitForData,       ok,     waitForChecksum},
//    {waitForData,       fail,   sendStatus},
//    {waitForChecksum,     ok,     sendStatus},
//    {waitForChecksum,     fail,   sendStatus},
//};

#define EXIT_STATE sendStatus
#define ENTRY_STATE waitForHeader

int processHeader() {
    return false;
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
    return sc;
}


int main(int argc, char *argv[]) {
    state_codes cur_state = ENTRY_STATE;
    ret_codes rc;
    int (* state_fun)(void);

    if (!UART_Init(BAUDRATE)) {
        // UART init returned 0 so failed
        
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

