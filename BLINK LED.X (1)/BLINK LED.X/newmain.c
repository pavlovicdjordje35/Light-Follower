/* 
 * File:   newmain.c
 * Author: pavlovicdjordje35
 *
 * Created on March 11, 2024, 8:57 PM
 */

#include <p30Fxxxx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"
#include "adc.h"
#include "pwm.h"
#include "uart.h"



_FOSC(CSW_FSCM_OFF & XT_PLL4);
_FWDT(WDT_OFF);

// IN1 , IN2 for right motor
#define IN1     LATBbits.LATB9 
#define IN2     LATBbits.LATB10
// IN3 , IN4 for front motor
#define IN3     LATBbits.LATB12
#define IN4     LATBbits.LATB11

// macros for echo and trig pins
#define TRIG_FRONT LATBbits.LATB4
#define ECHO_FRONT PORTBbits.RB5
#define TRIG_RIGHT LATBbits.LATB6
#define ECHO_RIGHT PORTBbits.RB7

#define FL PR[0]
#define FR PR[1]
#define BL PR[2]
#define BR PR[3]

/// Function for configuring pins for HC - SR04 sensors

#define MAX_ADC_VALUE (4095)
#define SPEED_OF_SOUND (0.0343) // centimeters per microsecond
#define INSTRUCTION_CLOCK_PERIOD (0.1) // microseconds
#define SIZE_OF_WORD (6)

unsigned int broj,broj1,broj2;
unsigned int sirovi0,sirovi1,sirovi2,sirovi3;
unsigned int broj,broj1,broj2,tempRX;
static float PR[4] = {0, 0, 0, 0};
static int us_counter, ms_counter;
static unsigned char tempRX1_debug, tempRX2_bluetooth;
static unsigned char word_start[SIZE_OF_WORD];
static unsigned char position_UART2 = 0;
static int duty_cycle_entered1;
static int duty_cycle_entered2;
static float measured_distance_front = 0;
static float measured_distance_right = 0;
static unsigned char time_overflow_front = 0;
static unsigned char time_overflow_right = 0;
static int def_speed = 70;
/// Interrupt Service Routine(ISR) for Timer1 - ms


void initUART1(void)
{
U1BRG=0x0040;//baud rate 9600
U1MODEbits.ALTIO = 0;
IEC0bits.U1RXIE = 1;
U1STA&=0xfffc;
U1MODEbits.UARTEN=1;
U1STAbits.UTXEN=1;
}
void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;
  //   tempRX=U1RXREG;

} 

void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
							

        PR[0] = ADCBUF0; // naponski razdelnik
        PR[1] = ADCBUF1;
        PR[2] = ADCBUF2;
        PR[3] = ADCBUF3;

    IFS0bits.ADIF = 0;

} 


void __attribute__((__interrupt__)) _T1Interrupt(void) {
    TMR1 = 0;
    ms_counter++;
    IFS0bits.T1IF = 0;
}

/// Interrupt Service Routine(ISR) for Timer2 - us

void __attribute__((__interrupt__)) _T2Interrupt(void) {
    TMR2 = 0;
    us_counter++;
    IFS0bits.T2IF = 0;
}

/// Interrupt Service Routine(ISR) for Timer3 - PWM

void __attribute__((__interrupt__)) _T3Interrupt(void) {
    TMR3 = 0;
    IFS0bits.T3IF = 0;
}

/// Interrupt Service Routine(ISR) for Timer4 - front position sensor

void __attribute__((__interrupt__)) _T4Interrupt(void) {
    TMR4 = 0;
    ECHO_FRONT = 0;
    time_overflow_front = 1;
    IFS1bits.T4IF = 0;
}

/// Interrupt Service Routine(ISR) for Timer5 - right position sensor

void __attribute__((__interrupt__)) _T5Interrupt(void) {
    TMR5 = 0;
    ECHO_RIGHT = 0;
    time_overflow_right = 1;
    IFS1bits.T5IF = 0;
}

void WriteUART1(unsigned int data)
{


	while (U1STAbits.TRMT==0);
    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}





void __attribute__((__interrupt__)) _U2RXInterrupt(void) {
    IFS1bits.U2RXIF = 0;
    tempRX2_bluetooth = U2RXREG;

    if (tempRX2_bluetooth != 0) {
        word_start[position_UART2] = tempRX2_bluetooth;
        tempRX2_bluetooth = 0;

        if (position_UART2 < SIZE_OF_WORD - 1) {
            position_UART2++;
            word_start[position_UART2] = 0;
        } else position_UART2 = 0;
    }
}



static void DelayMs(int vreme);

static void DelayUs(int vreme);

static void MeasureFrontDistance();

static void MeasureRightDistance();

static void ChangeDutyCycle();

static void CnfgM();

static void stopM();

static void driveF();

static void driveB();

static void front();

static void right();

static void ConfigureHCSR04Pins(void);

static void Calc(void);
/*
 * 
 */

int main(int argc, char** argv) {


//    ConfigureADCPins();
        /// Pin for sharp sensor
    ADPCFGbits.PCFG0=0;     // analog pin
    ADPCFGbits.PCFG1=0;     
    ADPCFGbits.PCFG2=0;     
    ADPCFGbits.PCFG3=0;     

    TRISBbits.TRISB0=1;     // input pin
    TRISBbits.TRISB1=1;     
    TRISBbits.TRISB2=1;     
    TRISBbits.TRISB3=1;  
    
    ADCinit();
    Init_T1();
    Init_T2();
    Init_T3();
    Init_T4();
    Init_T5();
    InitUART1();
    InitUART2();
    InitPWM();
    CnfgM();
    ConfigureHCSR04Pins();



    duty_cycle_entered1 = 75;
    DutyCyclePWM1(duty_cycle_entered1);
    duty_cycle_entered2 = 78;
    DutyCyclePWM2(duty_cycle_entered2);



    ADPCFGbits.PCFG4 = 1;
    ADPCFGbits.PCFG5 = 1;
    ADPCFGbits.PCFG6 = 1;
    ADPCFGbits.PCFG7 = 1;
    ADPCFGbits.PCFG9 = 1;
    ADPCFGbits.PCFG10 = 1;
    ADPCFGbits.PCFG11 = 1;
    ADPCFGbits.PCFG12 = 1;

    //------------ Iscekivanje START-a -----------------
    memset(word_start, 0, sizeof (word_start));
    position_UART2 = 0;
    WriteStringUART2("Write START.");
   // WriteCharUART2(13);
   /* while (word_start[0] != 'S'
            || word_start[1] != 'T'
            || word_start[2] != 'A'
            || word_start[3] != 'R'
            || word_start[4] != 'T'
            || word_start[5] != '\0'
            );
    WriteStringUART2("The car is started.");
    WriteCharUART2(13);*/
    ADCON1bits.ADON=1;

    driveF();
    while (1) {
      // Calc();
        //driveB();
       MeasureRightDistance();
       MeasureFrontDistance();
      /* if(measured_distance_right>10 && measured_distance_front>10 ){
       Calc(); 
        //   driveF();
       }*/
       Calc();
       if(measured_distance_right<10 && measured_distance_right>0 && measured_distance_front<10 && measured_distance_front>0){
       right();
       }
       if(measured_distance_right<10 && measured_distance_right>0 && measured_distance_front>10){
           driveF();
       }
       if(measured_distance_right>10 && measured_distance_front<10 && measured_distance_front>0){
           driveB();
       }
       
       
       //DelayMs(100);
       
       
       
       
       
       
       /*if(measured_distance_front<10 && measured_distance_front>0){
            right();
       }
            else 
                driveB();
       */
      
         /*  else if(measured_distance_front<10 && measured_distance_front>10)
           stopM();  */         
      
       /*if(measured_distance_front<10 && measured_distance_front>0){
           driveF(); 
       }*/
    }
        //driveF();
     
     








    return (EXIT_SUCCESS);
}

//---------------------------------------------------------

static void Calc(void){
    /*float max;
    int max_pos;
    int i = 0;
    for  (i = 0; i < 4; i++) {
        if (PR[i] > max) {
            max = PR[i];
            max_pos = i;
        }
    }
        switch (max_pos) {
            case 0:
                if ((PR[3] - PR[1]) > 0 ) {
                    duty_cycle_entered1 = 100;
                    duty_cycle_entered2 = 60;
                }else{
                    duty_cycle_entered1 = 80;
                    duty_cycle_entered2 = 80;
                }
                break;
            case 1:
                if ((PR[2] - PR[0]) > 0) {
                    duty_cycle_entered1=60;
                    duty_cycle_entered2=100;
                } else {
                    duty_cycle_entered1 = 80;
                    duty_cycle_entered2 = 80;
                }
                break;
            default:
                duty_cycle_entered1 = 80;
                    duty_cycle_entered2 = 80;
                if(measured_distance_front<15){
                    duty_cycle_entered1 = 80;
                    duty_cycle_entered2 = 80;}
                else{
                    duty_cycle_entered1=60;
                    duty_cycle_entered2=100;
                }
                    break;
            
        }
    */
   /* if(measured_distance_right < 20){
            driveF();
    }*/
    
    if(PR[1] > 3400 && PR[1]> PR[2] && PR[1] > PR[3]){
        driveB();
        duty_cycle_entered1 = 80;
        duty_cycle_entered2 = 80;
    }  
    else if(PR[0] > 1500 && PR[0]>(PR[2]-700) && PR[0] > (PR[3]-700)){
        driveF();
        duty_cycle_entered1 = 80;
        duty_cycle_entered2 = 80;
        }
    
    else if(PR[2] > 3300 && PR[2]>PR[0] && PR[2] > PR[1]){
        duty_cycle_entered1 = 50;
        duty_cycle_entered2 = 80;
    }
    else if(PR[3] > 3300 && PR[3]> PR[0] && PR[3] > PR[1]){
        duty_cycle_entered1 = 80;
        duty_cycle_entered2 = 50;
    }  
 
    
    DutyCyclePWM1(duty_cycle_entered1);
    DutyCyclePWM2(duty_cycle_entered2);
    




}

static void DelayMs(int vreme) {
    ms_counter = 0;
    T1CONbits.TON = 1;
    while (ms_counter < vreme);
    T1CONbits.TON = 0;
}

static void DelayUs(int vreme) {
    us_counter = 0;
    T2CONbits.TON = 1;
    while (us_counter < vreme);
    T2CONbits.TON = 0;
}

static void MeasureFrontDistance() {
    // logical one lasts for 10us

    TRIG_FRONT = 1;
    DelayUs(3); //  3 instead of 10 to make logical one lasts for 10us
    TRIG_FRONT = 0;
    DelayUs(3);
    while (!ECHO_FRONT); //  the value of the echo pin becomes 1 (the rising edge detected)
    TMR4 = 0; // reset T4
    IFS1bits.T4IF = 0;
    T4CONbits.TON = 1; // turn on T4, time measurement begins 
    while (ECHO_FRONT); // the value of the echo pin becomes 0 (the falling edge detected)
    T4CONbits.TON = 0; // turn off T4, time measurement stops
    unsigned int measured_time_front;
    if (time_overflow_front == 1) // time overflow happens
    {
        measured_time_front = TMR4_period;
        time_overflow_front = 0;
    } else // the signal sent has returned
    {
        measured_time_front = TMR4;
    }
    TMR4 = 0;
    // operation /2 is used because the ultrasonic pulse travels to the obstacle and back
    // operation *INSTRUCTION_CLOCK_PERIOD is used to get the time in microseconds
    measured_distance_front = (measured_time_front * INSTRUCTION_CLOCK_PERIOD) / 2 * SPEED_OF_SOUND;
}

static void MeasureRightDistance() {
    // logical one lasts for 10us
    TRIG_RIGHT = 1;
    DelayUs(3); //  3 instead of 10 to make logical one lasts for 10us
    TRIG_RIGHT = 0;
    DelayUs(3);
    while (!ECHO_RIGHT); //  the value of the echo pin becomes 1 (the rising edge detected)
    TMR5 = 0; // reset T5
    IFS1bits.T5IF = 0;
    T5CONbits.TON = 1; // turn on T5, time measurement begins 
    while (ECHO_RIGHT); // the value of the echo pin becomes 0 (the falling edge detected)
    T5CONbits.TON = 0; // turn off T4, time measurement stops
    unsigned int measured_time_right;
    if (time_overflow_right == 1) // time overflow happens
    {
        measured_time_right = TMR5_period;
        time_overflow_right = 0;
    } else // the signal sent has returned
    {
        measured_time_right = TMR5;
    }
    TMR5 = 0;
    // operation /2 is used because the ultrasonic pulse travels to the obstacle and back
    // operation *INSTRUCTION_CLOCK_PERIOD is used to get the time in microseconds
    measured_distance_right = (measured_time_right * INSTRUCTION_CLOCK_PERIOD) / 2 * SPEED_OF_SOUND;
}

void CnfgM() {
    TRISBbits.TRISB9 = 0; // output pin IN1
    TRISBbits.TRISB10 = 0; // output pin IN2
    TRISBbits.TRISB11 = 0; // output pin IN4
    TRISBbits.TRISB12 = 0; // output pin IN3

    ADPCFGbits.PCFG9 = 1; // digital pin IN1
    ADPCFGbits.PCFG10 = 1; // digital pin IN2
    ADPCFGbits.PCFG11 = 1; // digital pin IN4
    ADPCFGbits.PCFG12 = 1; // digital pin IN3
}

void stopM() {
    IN1 = 0;
    IN2 = 0;
    IN3 = 0;
    IN4 = 0;
}

void driveF() {
    IN1 = 1;
    IN2 = 0;
    IN3 = 0;
    IN4 = 1;
}

void driveB() {
    IN1 = 0;
    IN2 = 1;
    IN3 = 1;
    IN4 = 0;
}

void front() {
    IN1 = 0;
    IN2 = 0;
    IN3 = 1;
    IN4 = 0;
}

void right() {
    IN1 = 0;
    IN2 = 1;
    IN3 = 0;
    IN4 = 0;
}

void ConfigureHCSR04Pins() {
    TRISBbits.TRISB4 = 0; // output trigger pin for front position sensor
    TRISBbits.TRISB6 = 0; // output trigger pin for right position sensor
    TRISBbits.TRISB5 = 1; // input echo pin for front position sensor
    TRISBbits.TRISB7 = 1; // input echo pin for right position sensor

    TRIG_FRONT = 0;
    TRIG_RIGHT = 0;
    
}