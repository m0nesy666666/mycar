#include "Encoder.h"


//内部变量
volatile int Get_Encoder_countA = 0;
volatile int Get_Encoder_countB = 0;
volatile int Get_Encoder_countC = 0;
volatile int Get_Encoder_countD = 0;

// 全局变量定义 
volatile int32_t encoderA_cnt = 0, encoderB_cnt = 0, encoderC_cnt = 0, encoderD_cnt = 0;
volatile int32_t total_pulse_A = 0, total_pulse_B = 0, total_pulse_C = 0, total_pulse_D = 0;

volatile float speed_A = 0.0f, speed_B = 0.0f, speed_C = 0.0f, speed_D = 0.0f;
volatile float distance_A = 0.0f, distance_B = 0.0f, distance_C = 0.0f, distance_D = 0.0f;

// ================== 周期性更新函数 (20ms调用一次) ==================
void Encoder_Update(void)
{
    // ---------------- 1. 采集脉冲 ----------------
    encoderA_cnt = Get_Encoder_countA;
    encoderB_cnt = -Get_Encoder_countB; // B轮如果反装，加负号抵消
    encoderC_cnt = Get_Encoder_countC;
    encoderD_cnt = -Get_Encoder_countD; // D轮与B轮同侧，方向同样取反
    
    // 清空底层计数值，准备下个 20ms
    Get_Encoder_countA = 0;
    Get_Encoder_countB = 0;
    Get_Encoder_countC = 0;
    Get_Encoder_countD = 0;

    // ---------------- 2. 计算距离 ----------------
    // 累加总脉冲
    total_pulse_A += encoderA_cnt;
    total_pulse_B += encoderB_cnt;
    total_pulse_C += encoderC_cnt;
    total_pulse_D += encoderD_cnt;
    
    // 距离 = 总脉冲 * (轮子周长 / 转一圈的总脉冲)
    distance_A = total_pulse_A * (PI * WHEEL_DIAMETER_M / PULSE_PER_TURN);
    distance_B = total_pulse_B * (PI * WHEEL_DIAMETER_M / PULSE_PER_TURN);
    distance_C = total_pulse_C * (PI * WHEEL_DIAMETER_M / PULSE_PER_TURN);
    distance_D = total_pulse_D * (PI * WHEEL_DIAMETER_M / PULSE_PER_TURN);

    // ---------------- 3. 计算速度 ----------------
    // 20ms内位移 = 20ms内脉冲数 * (轮子周长 / 转一圈的总脉冲)
    // 速度(m/s) = 20ms内位移 / 0.02秒 = 20ms内位移 * 50.0
    speed_A = (encoderA_cnt * (PI * WHEEL_DIAMETER_M / PULSE_PER_TURN)) * 50.0f;
    speed_B = (encoderB_cnt * (PI * WHEEL_DIAMETER_M / PULSE_PER_TURN)) * 50.0f;
    speed_C = (encoderC_cnt * (PI * WHEEL_DIAMETER_M / PULSE_PER_TURN)) * 50.0f;
    speed_D = (encoderD_cnt * (PI * WHEEL_DIAMETER_M / PULSE_PER_TURN)) * 50.0f;
}

// ================== 外部中断服务函数 (脉冲计数) ==================
void GROUP1_IRQHandler(void)
{
    uint32_t gpio_interrup1 = DL_GPIO_getEnabledInterruptStatus(ENCODERA_PORT,ENCODERA_E1A_PIN|ENCODERA_E1B_PIN);
    uint32_t gpio_interrup2 = DL_GPIO_getEnabledInterruptStatus(ENCODERB_PORT,ENCODERB_E2A_PIN|ENCODERB_E2B_PIN);
    uint32_t gpio_interrup3 = DL_GPIO_getEnabledInterruptStatus(ENCODERC_PORT,ENCODERC_E3A_PIN|ENCODERC_E3B_PIN);
    uint32_t gpio_interrup4 = DL_GPIO_getEnabledInterruptStatus(ENCODERD_PORT,ENCODERD_E4A_PIN|ENCODERD_E4B_PIN);
    
    //encoderA
    if((gpio_interrup1 & ENCODERA_E1A_PIN)==ENCODERA_E1A_PIN)
    {
        if(!DL_GPIO_readPins(ENCODERA_PORT,ENCODERA_E1B_PIN))
            Get_Encoder_countA--;
        else
            Get_Encoder_countA++;
        DL_GPIO_clearInterruptStatus(ENCODERA_PORT,ENCODERA_E1A_PIN);
    }
    else if((gpio_interrup1 & ENCODERA_E1B_PIN)==ENCODERA_E1B_PIN)
    {
        if(!DL_GPIO_readPins(ENCODERA_PORT,ENCODERA_E1A_PIN))
            Get_Encoder_countA++;
        else
            Get_Encoder_countA--;
        DL_GPIO_clearInterruptStatus(ENCODERA_PORT,ENCODERA_E1B_PIN);
    }
    
    //encoderB
    if((gpio_interrup2 & ENCODERB_E2A_PIN)==ENCODERB_E2A_PIN)
    {
        if(!DL_GPIO_readPins(ENCODERB_PORT,ENCODERB_E2B_PIN))
            Get_Encoder_countB--;
        else
            Get_Encoder_countB++;
        DL_GPIO_clearInterruptStatus(ENCODERB_PORT,ENCODERB_E2A_PIN);
    }
    else if((gpio_interrup2 & ENCODERB_E2B_PIN)==ENCODERB_E2B_PIN)
    {
        if(!DL_GPIO_readPins(ENCODERB_PORT,ENCODERB_E2A_PIN))
            Get_Encoder_countB++;                
        else              
            Get_Encoder_countB--;
        DL_GPIO_clearInterruptStatus(ENCODERB_PORT,ENCODERB_E2B_PIN);
    }

    //encoderC
    if((gpio_interrup3 & ENCODERC_E3A_PIN)==ENCODERC_E3A_PIN)
    {
        if(!DL_GPIO_readPins(ENCODERC_PORT,ENCODERC_E3B_PIN))
            Get_Encoder_countC--;
        else
            Get_Encoder_countC++;
        DL_GPIO_clearInterruptStatus(ENCODERC_PORT,ENCODERC_E3A_PIN);
    }
    else if((gpio_interrup3 & ENCODERC_E3B_PIN)==ENCODERC_E3B_PIN)
    {
        if(!DL_GPIO_readPins(ENCODERC_PORT,ENCODERC_E3A_PIN))
            Get_Encoder_countC++;
        else
            Get_Encoder_countC--;
        DL_GPIO_clearInterruptStatus(ENCODERC_PORT,ENCODERC_E3B_PIN);
    }

    //encoderD
    if((gpio_interrup4 & ENCODERD_E4A_PIN)==ENCODERD_E4A_PIN)
    {
        if(!DL_GPIO_readPins(ENCODERD_PORT,ENCODERD_E4B_PIN))
            Get_Encoder_countD--;
        else
            Get_Encoder_countD++;
        DL_GPIO_clearInterruptStatus(ENCODERD_PORT,ENCODERD_E4A_PIN);
    }
    else if((gpio_interrup4 & ENCODERD_E4B_PIN)==ENCODERD_E4B_PIN)
    {
        if(!DL_GPIO_readPins(ENCODERD_PORT,ENCODERD_E4A_PIN))
            Get_Encoder_countD++;
        else
            Get_Encoder_countD--;
        DL_GPIO_clearInterruptStatus(ENCODERD_PORT,ENCODERD_E4B_PIN);
    }
    
   
}
