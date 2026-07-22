#include "ti_msp_dl_config.h"
#include "main.h"
#include "PID.h"
#include "Encoder.h"
#include "motor.h"
#include "Serial.h"
#include <stdio.h>
PID_t speed_pid_A;
PID_t speed_pid_B;
PID_t speed_pid_C;
PID_t speed_pid_D;
#define SPEED_KP  18.0f
#define SPEED_KI  0.5f
uint8_t oled_buffer[32];
int status = 0;

volatile float target_speed_A = 0.3f;
volatile float target_speed_B = 0.3f;
volatile float target_speed_C = 0.3f;
volatile float target_speed_D = 0.3f;

volatile float pwm_A;
volatile float pwm_B;
volatile float pwm_C;
volatile float pwm_D;
int main(void)
{
    SYSCFG_DL_init();
    SysTick_Init();
    // sudu0.Kp=1;
    // MPU6050_Init();
    OLED_Init();
    // Ultrasonic_Init();
    BNO08X_Init();
    
    /* Don't remove this! */
    Interrupt_Init();   
    motor_init();
    //DL_UART_Main_enableInterrupt(UART_DEBUG_INST, DL_UART_MAIN_INTERRUPT_RX);
    NVIC_ClearPendingIRQ(UART_DEBUG_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_DEBUG_INST_INT_IRQN);
    //  NVIC_EnableIRQ(ENCODERA_INT_IRQN );
    //   NVIC_EnableIRQ(ENCODERB_INT_IRQN );
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    DL_Timer_startCounter(TIMER_0_INST);
    //清除定时器中断标志
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN );
    
    OLED_ShowString(0,0,(uint8_t *)"speed_A=",8);
    OLED_ShowString(0,2,(uint8_t *)"target_A=",8);
    OLED_ShowString(0,4,(uint8_t *)"SPEED_KP=",8);
    OLED_ShowString(0,6,(uint8_t *)"SPEED_KI=",8);
    OLED_ShowString(0,8,(uint8_t *)"pwm_A=",8);
    OLED_ShowString(0,10,(uint8_t *)"BNO08X Demo",8);
    
    
    

    PID_Init(&speed_pid_A, SPEED_KP, SPEED_KI, 0.0f, 100.0f, -100.0f);
    PID_Init(&speed_pid_B, SPEED_KP, SPEED_KI, 0.0f, 100.0f, -100.0f);
    PID_Init(&speed_pid_C, SPEED_KP, SPEED_KI, 0.0f, 100.0f, -100.0f);
    PID_Init(&speed_pid_D, SPEED_KP, SPEED_KI, 0.0f, 100.0f, -100.0f);
    
    while (1) 
    {   
        sprintf((char *)oled_buffer, "%-6.3f  ", speed_A);
        OLED_ShowString(80,0,oled_buffer,8);
        sprintf((char *)oled_buffer, "%-6.3f  ", target_speed_A);
        OLED_ShowString(80,2,oled_buffer,8);
        sprintf((char *)oled_buffer, "%-6.3f  ", SPEED_KP);
        OLED_ShowString(80,4,oled_buffer,8);
        sprintf((char *)oled_buffer, "%-6.3f  ", SPEED_KI);
        OLED_ShowString(80,6,oled_buffer,8);
        sprintf((char *)oled_buffer, "%-6.3f  ",  pwm_A);
        OLED_ShowString(80,8,oled_buffer,8);
        
         
        // OLED_ShowString(0, 6, (uint8_t *)Serial_RxString, 8);
       
        
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
           
                
            
            Encoder_Update();                                //实际值，目标值
             pwm_A = PID_Positional_Update(&speed_pid_A, speed_A, target_speed_A);
             pwm_B = PID_Positional_Update(&speed_pid_B, speed_B, target_speed_B);
             pwm_C = PID_Positional_Update(&speed_pid_C, speed_C, target_speed_C);
             pwm_D = PID_Positional_Update(&speed_pid_D, speed_D, target_speed_D);
            Motor_SetPWM(1, (int8_t)pwm_A);
            Motor_SetPWM(2, (int8_t)pwm_B);
            Motor_SetPWM(3, (int8_t)pwm_C);
            Motor_SetPWM(4, (int8_t)pwm_D);
            
            break;
           }
        default:
        break;
    }
}
