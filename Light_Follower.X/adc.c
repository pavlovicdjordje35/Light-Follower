/* 
 * File:   adc.c
 * Author: pavlovicdjordje35
 *
 * Created on March 11, 2024, 10:08 PM
 */

#include <p30Fxxxx.h>


#include "adc.h"


void ConfigureADCPins(void)
{
    /// Pin for sharp sensor
    ADPCFGbits.PCFG1=0;     // analog pin  
    TRISBbits.TRISB1=1;     // input pin
}   


void InitADC(void)
{
    /// 0 = AD conversion continue module operation in Idle mode
    ADCON1bits.ADSIDL=0; 
    /// 00 = Data Output Format bits is integer (DOUT = 0000 dddd dddd dddd)
    ADCON1bits.FORM=0; 
    /// 111 = Internal counter ends sampling and starts conversion
    ADCON1bits.SSRC=7; 
    /// 1 = At least one A/D sample/hold amplifier is sampling
    ADCON1bits.SAMP=3; 

    // Voltage Reference Configuration bits : 1xx - VREFH=AVDD, VREFL=AVSS
    ADCON2bits.VCFG=7; 
    /// 1 = Scan inputs Selections for CH0+ S/H Input for MUX A Input Multiplexer Setting bit
    ADCON2bits.CSCNA=1; 
    /// 0011 = Interrupts at the completion of conversion for each 4th sample/convert sequence
    ADCON2bits.SMPI=0;
    /// 0 = Buffer configured as one 16-word buffer ADCBUF(15...0)
    ADCON2bits.BUFM=0;
    /// 0 = Always use MUX A input multiplexer settings for all samples
    ADCON2bits.ALTS=0;

    /// 11111 = 31 TAD, Auto Sample Time bits
    ADCON3bits.SAMC=31; 
    /// 1 = A/D internal RC clock
    ADCON3bits.ADRC=1;
    /// 111111 = TCY/2 * (ADCS<5:0> + 1) = 32 * TCY, TCY is instructional tact
    ADCON3bits.ADCS=31;
    
    /// 0 = Channel 0 negative input is VREFbit (for MUX B Multiplexer Setting bit)
    ADCHSbits.CH0NB=0;
    /// 0 = Channel 0 negative input is VREFbit (for MUX A Multiplexer Setting bit)
    ADCHSbits.CH0NA=0;
    /// 0000 = Channel 0 positive input is AN0 (for MUX A Multiplexer Setting bit)
    ADCHSbits.CH0SA=0;
    /// 0000 = Channel 0 positive input is AN0 (for MUX B Multiplexer Setting bit)
    ADCHSbits.CH0SB=0;

    /// 1 = Select ANx for input scan, 0 = Skip ANx for input scan
    ADCSSL=0b0000000000001111; 
    /// 1 - Starts sampling after the last finished conversion
    ADCON1bits.ASAM=1; 
    /// AD conversion interrupt flag is up
    IFS0bits.ADIF=1; 
    /// AD conversion interrupt enable is enabled
    IEC0bits.ADIE=1;
}


