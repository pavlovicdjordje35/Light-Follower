/* 
 * File:   uart.c
 * Author: pavlovicdjordje35
 *
 * Created on March 11, 2024, 9:10 PM
 */

#include <p30Fxxxx.h>

#include "uart.h"





void InitUART1(void)
{
    /// U1BRG = (Fcy / (16 * Baudrate) - 1
    U1BRG=0x0040;           /// Baudrate = 9600, Fcy = 10MHz, U1BRG = 40
    U1MODEbits.ALTIO=0;     /// 1 = alternative UART1 pins is used
    IEC0bits.U1RXIE=1;      /// 1 = enabled RX interupt for UART1
    U1STA&=0xfffc;          
    U1MODEbits.UARTEN=1;    /// 1 = enable UART1 module
    U1STAbits.UTXEN=1;      /// 1 = enabled TX for UART1
}

void WriteCharUART1(unsigned int data)
{
    while(!U1STAbits.TRMT);         
    if(U1MODEbits.PDSEL == 3)      
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

void WriteStringUART1(register const char *str)
{
    while((*str)!=0)
    {
        WriteCharUART1(*str);
        if(*str == 10)
            WriteCharUART1(13);
        if(*str == 13)
            WriteCharUART1(10);
        str++;
    }
}

void WriteUART1dec2string(unsigned int data)
{
    unsigned char temp;
    temp=data/1000;
    WriteCharUART1(temp+'0');  
    data=data-temp*1000;
    temp=data/100;
    WriteCharUART1(temp+'0');
    data=data-temp*100;
    temp=data/10;
    WriteCharUART1(temp+'0');
    data=data-temp*10;
    WriteCharUART1(data+'0');
}

/**UART2-------------------------------------*/

void InitUART2(void)
{
    /// U2BRG = (Fcy / (16 * Baudrate) - 1
    U2BRG=0x0040;           /// Baudrate = 9600, Fcy = 10MHz, U2BRG = 40
    IEC1bits.U2RXIE=1;      /// 1 = enabled RX interupt for UART2
    U2STA&=0xfffc;          
    U2MODEbits.UARTEN=1;    /// 1 = enable UART2 module
    U2STAbits.UTXEN=1;      /// 1 = enabled TX for UART2
}

void WriteCharUART2(unsigned int data)
{
    while(!U2STAbits.TRMT);         /// Waiting for the transmit register to be available
    if(U2MODEbits.PDSEL == 3)       /// 9-bit data without parity bit
        U2TXREG = data;
    else
        U2TXREG = data & 0xFF;
}

void WriteStringUART2(register const char *str)
{
    while((*str)!=0)
    {
        WriteCharUART2(*str++);
    }
}

void WriteUART2dec2string(unsigned int data)
{
    unsigned char temp;
    temp=data/1000;
    WriteCharUART2(temp+'0');  
    data=data-temp*1000;
    temp=data/100;
    WriteCharUART2(temp+'0');
    data=data-temp*100;
    temp=data/10;
    WriteCharUART2(temp+'0');
    data=data-temp*10;
    WriteCharUART2(data+'0');
}
