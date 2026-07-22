#ifndef MOTOR_H
#define MOTOR_H

#include "ti_msp_dl_config.h"



void PWM_SetCompare0(uint16_t Compare0);
void PWM_SetCompare1(uint16_t Compare1);
void PWM_SetCompare2(uint16_t Compare2);
void PWM_SetCompare3(uint16_t Compare3);
void motor_init();
void Motor_SetPWM(uint8_t n, int8_t PWM);

#endif
