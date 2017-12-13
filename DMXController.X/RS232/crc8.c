/* 
 * File:   crc8.c
 * Author: User
 *
 * Created on 12 December 2017, 2:49 PM
 */

#include "crc8.h"

unsigned int crc8_update(unsigned char ser_data)
{
    // Update the CRC for transmitted and received data using
    // the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1).
    static unsigned int crc;
    crc = (unsigned char)(crc >> 8) | (crc << 8); 
    crc ^= ser_data;
    crc ^= (unsigned char)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;
    return crc;
}