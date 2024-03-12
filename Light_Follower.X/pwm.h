/* 
 * File:   pwm.h
 * Author: pavlovicdjordje35
 *
 * Created on March 11, 2024, 10:11 PM
 */

#ifndef PWM_H
#define	PWM_H

#include <p30Fxxxx.h>

void InitPWM();

void DutyCyclePWM1(int duty_cycle);

void DutyCyclePWM2(int duty_cycle);

#endif	/* PWM_H */

