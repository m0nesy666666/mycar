#ifndef __PID_H
#define __PID_H

#include "ti_msp_dl_config.h"

typedef struct {
    volatile float Target;       // 内部目标值
    float Actual;       // 内部实际值
    float Out;          // 控制器输出

    float Kp;
    float Ki;
    float Kd;

    float Error0;       // 当前误差 e(k)
    float Error1;       // 上次误差 e(k-1)
    float Error2;       // 上上次误差 e(k-2) -> 增量式专用
    float ErrorInt;     // 误差积分 -> 位置式专用
    float ErrorIntMax;  // 积分限幅绝对值 (新增加)

    float OutMax;       // 输出上限
    float OutMin;       // 输出下限
} PID_t;

void PID_Init(PID_t *p, float kp, float ki, float kd, float out_max, float out_min);
float PID_Positional_Update(PID_t *p, float actual, float target);
float PID_Incremental_Update(PID_t *p, float actual, float target);


#endif