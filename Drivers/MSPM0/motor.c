#include "ti_msp_dl_config.h"
#include "motor.h"
#include "PID.h"
void PWM_SetCompare0(uint16_t Compare0) // PA0 PWMA
{
    if(Compare0>100)Compare0 = 100;
    DL_Timer_setCaptureCompareValue(PWMABCD_INST,Compare0,GPIO_PWMABCD_C0_IDX);
}

void PWM_SetCompare1(uint16_t Compare1) // PA1 PWMB
{
    if(Compare1>100)Compare1 = 100;
    DL_Timer_setCaptureCompareValue(PWMABCD_INST,Compare1,GPIO_PWMABCD_C1_IDX);
}

void PWM_SetCompare2(uint16_t Compare2) // PA3 PWMC
{
    if(Compare2>100)Compare2 = 100;
    DL_Timer_setCaptureCompareValue(PWMABCD_INST,Compare2,GPIO_PWMABCD_C2_IDX);
}

void PWM_SetCompare3(uint16_t Compare3) // PA4 PWMD
{
    if(Compare3>100)Compare3 = 100;
    DL_Timer_setCaptureCompareValue(PWMABCD_INST,Compare3,GPIO_PWMABCD_C3_IDX);
}

void motor_init()
{   
    DL_GPIO_setPins(DC_MOTOR_STBY_PORT, DC_MOTOR_STBY_PIN);      

    DL_GPIO_setPins(DC_MOTOR_AIN1_PORT, DC_MOTOR_AIN1_PIN);
    DL_GPIO_setPins(DC_MOTOR_AIN2_PORT, DC_MOTOR_AIN2_PIN);
    DL_Timer_setCaptureCompareValue(PWMABCD_INST, 0, GPIO_PWMABCD_C0_IDX);
   
    DL_GPIO_setPins(DC_MOTOR_BIN1_PORT, DC_MOTOR_BIN1_PIN);
    DL_GPIO_setPins(DC_MOTOR_BIN2_PORT, DC_MOTOR_BIN2_PIN);
    DL_Timer_setCaptureCompareValue(PWMABCD_INST, 0, GPIO_PWMABCD_C1_IDX);

    DL_GPIO_setPins(DC_MOTOR_CIN1_PORT, DC_MOTOR_CIN1_PIN);
    DL_GPIO_setPins(DC_MOTOR_CIN2_PORT, DC_MOTOR_CIN2_PIN);
    DL_Timer_setCaptureCompareValue(PWMABCD_INST, 0, GPIO_PWMABCD_C2_IDX);

    DL_GPIO_setPins(DC_MOTOR_DIN1_PORT, DC_MOTOR_DIN1_PIN);
    DL_GPIO_setPins(DC_MOTOR_DIN2_PORT, DC_MOTOR_DIN2_PIN);
    DL_Timer_setCaptureCompareValue(PWMABCD_INST, 0, GPIO_PWMABCD_C3_IDX);

    DL_Timer_startCounter(PWMABCD_INST);
    // DL_Timer_startCounter(MOTOR_PID_INST);
    // NVIC_EnableIRQ(MOTOR_PID_INST_INT_IRQN);
}
void Motor_SetPWM(uint8_t n, int8_t PWM)
{
	if (n == 1)
	{
		if (PWM >= 0)
		{
            DL_GPIO_setPins(DC_MOTOR_AIN1_PORT, DC_MOTOR_AIN1_PIN);
            DL_GPIO_clearPins(DC_MOTOR_AIN2_PORT,DC_MOTOR_AIN2_PIN);
			PWM_SetCompare0(PWM);
		}
		else
		{
            DL_GPIO_setPins(DC_MOTOR_AIN2_PORT,DC_MOTOR_AIN2_PIN);
            DL_GPIO_clearPins(DC_MOTOR_AIN1_PORT, DC_MOTOR_AIN1_PIN);
			PWM_SetCompare0(-PWM);
		}
	}
	else if (n == 2)
	{
		if (PWM >= 0)
		{
            DL_GPIO_setPins(DC_MOTOR_BIN2_PORT, DC_MOTOR_BIN2_PIN);
            DL_GPIO_clearPins(DC_MOTOR_BIN1_PORT,DC_MOTOR_BIN1_PIN);
			PWM_SetCompare1(PWM);
		}
		else
		{			
            DL_GPIO_setPins(DC_MOTOR_BIN1_PORT,DC_MOTOR_BIN1_PIN);
            DL_GPIO_clearPins(DC_MOTOR_BIN2_PORT, DC_MOTOR_BIN2_PIN);

			PWM_SetCompare1(-PWM);
		}
	}
	else if (n == 3)
	{
		if (PWM >= 0)
		{
            DL_GPIO_setPins(DC_MOTOR_CIN1_PORT, DC_MOTOR_CIN1_PIN);
            DL_GPIO_clearPins(DC_MOTOR_CIN2_PORT, DC_MOTOR_CIN2_PIN);
			PWM_SetCompare2(PWM);
		}
		else
		{
            DL_GPIO_setPins(DC_MOTOR_CIN2_PORT, DC_MOTOR_CIN2_PIN);
            DL_GPIO_clearPins(DC_MOTOR_CIN1_PORT, DC_MOTOR_CIN1_PIN);
			PWM_SetCompare2(-PWM);
		}
	}
	else if (n == 4)
	{
		if (PWM >= 0)
		{
            DL_GPIO_setPins(DC_MOTOR_DIN1_PORT, DC_MOTOR_DIN1_PIN);
            DL_GPIO_clearPins(DC_MOTOR_DIN2_PORT, DC_MOTOR_DIN2_PIN);
			PWM_SetCompare3(PWM);
		}
		else
		{
            DL_GPIO_setPins(DC_MOTOR_DIN2_PORT, DC_MOTOR_DIN2_PIN);
            DL_GPIO_clearPins(DC_MOTOR_DIN1_PORT, DC_MOTOR_DIN1_PIN);
			PWM_SetCompare3(-PWM);
		}
	}
}


