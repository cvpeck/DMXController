/*
 * File:   debug.c
 * Author: peckc
 *
 * Created on March 18, 2017, 9:39 AM
 */


#include <xc.h>
#include <stdbool.h>
#include "commands.h"
#include "debug.h"



void putch(unsigned char data) {
    while (! PIR1bits.TXIF)
        continue;
    TXREG = data;
}

void init_debug_uart(void) {
    TXSTAbits.TXEN = 1;
    RCSTAbits.SPEN = 1;
}

void initialise_diagnostics(struct diagnostics *diagnostic_options)
{
    diagnostic_options->all_echo = DIAGNOSTICS_ECHO_ALL;
    diagnostic_options->echo_command = DIAGNOSTICS_ECHO_COMMAND;
    diagnostic_options->echo_header = DIAGNOSTICS_ECHO_HEADER;
    diagnostic_options->echo_data_length = DIAGNOSTICS_ECHO_DATA_LENGTH;
    diagnostic_options->echo_data = DIAGNOSTICS_ECHO_DATA;
}