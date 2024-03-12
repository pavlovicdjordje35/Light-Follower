/* 
 * File:   uart.h
 * Author: pavlovicdjordje35
 *
 * Created on March 11, 2024, 9:10 PM
 */

#ifndef UART_H
#define	UART_H

#include <p30Fxxxx.h>

void InitUART1(void);
void WriteCarUART1(unsigned int data);
void WriteStringUART1(register const char *str);
void WriteUART1dec2string(unsigned int data);

void InitUART2(void);
void WriteCarUART2(unsigned int data);
void WriteStringUART2(register const char *str);
void WriteUART2dec2string(unsigned int data);

#endif	/* UART_H */

