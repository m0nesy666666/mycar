#ifndef _ENCODER_H
#define _ENCODER_H
#include "ti_msp_dl_config.h"

#define PI                  3.1415926f
#define WHEEL_DIAMETER_M    0.068f      // 轮子直径 68mm = 0.068m
#define ENCODER_RESOLUTION  13.0f       // 编码器本身线数
#define GEAR_RATIO          28.0f       // 减速比 1:28
#define MULTIPLE            2.0f        // A/B两相信号上升沿计数，等效2倍频

// 轮子转一圈的总脉冲数 = 13 * 28 * 2 = 728
#define PULSE_PER_TURN      (ENCODER_RESOLUTION * GEAR_RATIO * MULTIPLE) 

// ================== 测速与测距外部变量声明 ==================
extern volatile int32_t encoderA_cnt, encoderB_cnt, encoderC_cnt, encoderD_cnt;
extern volatile int32_t total_pulse_A, total_pulse_B, total_pulse_C, total_pulse_D;

extern volatile float speed_A, speed_B, speed_C, speed_D;
extern volatile float distance_A, distance_B, distance_C, distance_D;

// ================== 对外接口函数 ==================
void Encoder_Update(void); // 周期性更新函数（放在20ms定时器中调用）
#endif
