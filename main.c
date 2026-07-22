#include "ti_msp_dl_config.h"
#include "main.h"
#include "PID.h"
#include "Encoder.h"
#include "motor.h"
#include "Serial.h"
#include <stdio.h>
PID_t speed_pid;

uint8_t oled_buffer[32];
int status = 0;
float sudu=0;
float target =0.5;
int8_t out;
int8_t count;
/* 目标速度，单位m/s */
volatile float target_speed = 0.5f;

/* 两个电机的平均实际速度，单位m/s */
volatile float average_speed = 0.0f;

/* PID输出，范围-100～100 */
volatile float base_pwm = 0.0f;
int main(void)
{
    SYSCFG_DL_init();
    SysTick_Init();
    // sudu0.Kp=1;
    // MPU6050_Init();
    OLED_Init();
    // Ultrasonic_Init();
    BNO08X_Init();
    PID_Init((&speed_pid), 10.0, 10.0, 0.0, 100, -100);
    /* Don't remove this! */
    Interrupt_Init();   
    DL_Timer_startCounter(PWM_0_INST);          //步进电机1PWM
    DL_Timer_startCounter(PWM_1_INST);          //步进电机2PWM
    motor_init();
     //DL_UART_Main_enableInterrupt(UART_DEBUG_INST, DL_UART_MAIN_INTERRUPT_RX);
    NVIC_ClearPendingIRQ(UART_DEBUG_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_DEBUG_INST_INT_IRQN);
    //  NVIC_EnableIRQ(ENCODERA_INT_IRQN );
    //   NVIC_EnableIRQ(ENCODERB_INT_IRQN );
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    DL_Timer_startCounter(TIMER_0_INST);
    NVIC_EnableIRQ(UART_STEPMOTOR1_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_STEPMOTOR2_INST_INT_IRQN);
    
    //清除定时器中断标志
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN );
    
    int PWM1;
    int PWM2;
    int pwm3;
    int pwm4;
    int PWM45;
    int32_t COUNT;
    OLED_ShowString(0,7,(uint8_t *)"BNO08X Demo",8);
    OLED_ShowString(0,1,(uint8_t *)"BNO08X Demo",8);

    // PWM2=Calculate_target(15);
    // Set_PWM(PWM1,PWM2);
    // Motor_SetPWM(0,80);
    // Motor_SetPWM(1,80);
    
    
    
    while (1) 
    {   
        
        
         
        // OLED_ShowString(0, 6, (uint8_t *)Serial_RxString, 8);
        // // Motor_SetPWM(0,25);
        // sudu=speed_A ;
        // sprintf((char *)oled_buffer, "%-6.3f  ", sudu);
        // OLED_ShowString(6,6,oled_buffer,8);
        // sprintf((char *)oled_buffer, "Kp:%-6.2f     ", sudu0.Kp);
        // OLED_ShowString(40, 4, oled_buffer, 8);
        // sprintf((char *)oled_buffer, "Ki:%-6.2f     ", sudu0.Ki);
        // OLED_ShowString(40, 2, oled_buffer, 8);

        // OLED_ShowString(0, 0, (uint8_t *)"RX:", 8);
        
        // OLED_ShowString(24, 0, (uint8_t *)Serial_RxString, 8);
        // Serial_SendByte(0x01);
        // Serial_Printf("%.3f,%.3f,%.3f\r\n",sudu,target,sudu0.Out);
        // if (Serial_RxFlag == 1)
        // {
        //     Serial_RxFlag = 0;
        //     float temp_kp = 0.0f;
        //     float temp_ki = 0.0f;
        //     float temp_target = 0.0f;
        //     if (sscanf(Serial_RxString, "P:%f", &temp_kp) == 1) 
        //     {
        //         sudu0.Kp = temp_kp;

        //         sudu0.Out =0.0f;
        //         sudu0.Error0 = 0.0f;
        //         sudu0.Error1 = 0.0f;
        //         sudu0.Error2 = 0.0f;
        //         sudu0.ErrorInt = 0.0f;
                
        //     }
        //     if (sscanf(Serial_RxString, "I:%f", &temp_ki) == 1) 
        //     {
        //         sudu0.Ki = temp_ki; 
        //         sudu0.Out =0.0f;
        //         sudu0.Error0 = 0.0f;
        //         sudu0.Error1 = 0.0f;
        //         sudu0.Error2 = 0.0f;
        //         sudu0.ErrorInt = 0.0f;
        //     }
        //     if (sscanf(Serial_RxString, "T:%f", &temp_target) == 1)
        //     {
        //         target = temp_target;

                
        //         sudu0.Error0 = 0.0f;
        //         sudu0.Error1 = 0.0f;
        //         sudu0.Error2 = 0.0f;
        //         sudu0.ErrorInt = 0.0f;
        //     }
        // }
        // sprintf((char *)oled_buffer, "%-6.1f", bno08x_data.pitch);
        // OLED_ShowString(5*8,0,oled_buffer,16);
        // sprintf((char *)oled_buffer, "%-6.1f", bno08x_data.roll);
        // OLED_ShowString(5*8,2,oled_buffer,16);
        // sprintf((char *)oled_buffer, "%-6.1f", bno08x_data.yaw);
        // OLED_ShowString(5*8,4,oled_buffer,16);

        // sprintf((char *)oled_buffer, "%6d", bno08x_data.ax);
        // OLED_ShowString(15*6,1,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", bno08x_data.ay);
        // OLED_ShowString(15*6,2,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", bno08x_data.az);
        // OLED_ShowString(15*6,3,oled_buffer,8);
    }
}
void TIMER_0_INST_IRQHandler()
{
    switch (DL_Timer_getPendingInterrupt(TIMER_0_INST))
    {
        case DL_TIMER_IIDX_ZERO:
           {
            
            
               
            DL_GPIO_togglePins(LED_PORT, LED_PIN_0_PIN);
           
                
            
            Encoder_Update();
            //base_pwm=pid.out
            average_speed = (speed_A + speed_B) / 2.0f;
            base_pwm= PID_Positional_Update(&speed_pid,average_speed,target_speed);
            // DL_GPIO_togglePins(LED_PORT ,LED_PIN_0_PIN);
            Motor_SetPWM(0, (int8_t) base_pwm);
            Motor_SetPWM(1, (int8_t) base_pwm);
            break;
           }
        default:
        break;
    }
}
