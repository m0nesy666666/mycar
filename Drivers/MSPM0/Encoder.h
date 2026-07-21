#ifndef _ENCODER_H
#define _ENCODER_H
#include "ti_msp_dl_config.h"

#define PI                  3.1415926f
#define WHEEL_DIAMETER_M    0.068f      // 轮子直径 68mm = 0.068m
#define ENCODER_RESOLUTION  13.0f       // 编码器本身线数
#define GEAR_RATIO          28.0f       // 减速比 1:28
#define MULTIPLE            2.0f        // 4倍频 (Any Edge双边沿触发)

// 轮子转一圈的总脉冲数 = 13 * 20 * 4 = 1040
#define PULSE_PER_TURN      (ENCODER_RESOLUTION * GEAR_RATIO * MULTIPLE) 

// ================== 测速与测距外部变量声明 ==================
extern volatile int32_t encoderA_cnt, encoderB_cnt;        // 10ms内的脉冲数
extern volatile int32_t total_pulse_A, total_pulse_B;      // 累计总脉冲数（测距用）

extern volatile float speed_A, speed_B;              // 瞬时速度 (单位: m/s)
extern volatile float distance_A, distance_B;              // 累计位移 (单位: m)

// ================== 对外接口函数 ==================
void Encoder_Update(void); // 周期性更新函数（放在10ms定时器中调用）
#endif