#ifndef MOTOR_H
#define MOTOR_H

#include "ti_msp_dl_config.h"



void PWM_SetCompare0(uint16_t Compare0);
void PWM_SetCompare1(uint16_t Compare1);
void motor_init();
void Motor_SetPWM(uint8_t n, int8_t PWM);

#endif