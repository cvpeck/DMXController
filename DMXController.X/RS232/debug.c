/*
 * File:   debug.c
 * Author: peckc
 *
 * Created on March 18, 2017, 9:39 AM
 */


#include <xc.h>
#include <stdbool.h>


void putch(unsigned char data) {
    while (! PIR1bits.TXIF)
        continue;
    TXREG = data;
}

void init_debug_uart(void) {
    TXSTAbits.TXEN = 1;
    RCSTAbits.SPEN = 1;
}