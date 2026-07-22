#include "PID.h"
#include <math.h>   

void PID_Init(PID_t *p, float kp, float ki, float kd, float out_max, float out_min)
{
    p->Kp = kp;
    p->Ki = ki;
    p->Kd = kd;
    p->OutMax = out_max;
    p->OutMin = out_min;

    p->ErrorIntMax = 20.0f;

    // 清零动态变量
    p->Target   = 0.0f;
    p->Actual   = 0.0f;
    p->Out      = 0.0f;
    p->Error0   = 0.0f;
    p->Error1   = 0.0f;
    p->Error2   = 0.0f;
    p->ErrorInt = 0.0f;
}



float PID_Positional_Update(PID_t *p, float actual, float target)
{
    p->Actual = actual;
    p->Target = target;

    // 更新误差
    p->Error1 = p->Error0;
    p->Error0 = p->Target - p->Actual;

    // 积分累加 + 限幅
    if (p->Ki != 0.0f) {
        p->ErrorInt += p->Error0;

        // 积分限幅，防止饱和
        if (p->ErrorInt > p->ErrorIntMax)  p->ErrorInt = p->ErrorIntMax;
        if (p->ErrorInt < -p->ErrorIntMax) p->ErrorInt = -p->ErrorIntMax;
    } else {
        p->ErrorInt = 0.0f;
    }

    // 位置式 PID 计算
    p->Out = p->Kp * p->Error0
           + p->Ki * p->ErrorInt
           + p->Kd * (p->Error0 - p->Error1);

    // 输出限幅
    if (p->Out > p->OutMax) p->Out = p->OutMax;
    if (p->Out < p->OutMin) p->Out = p->OutMin;

    return p->Out;
}

float PID_Incremental_Update(PID_t *p, float actual, float target)
{
    p->Actual = actual;
    p->Target = target;

    // 误差平移
    p->Error2 = p->Error1;
    p->Error1 = p->Error0;
    p->Error0 = p->Target - p->Actual;

    // 计算增量
    float delta = p->Kp * (p->Error0 - p->Error1)
                + p->Ki * p->Error0
                + p->Kd * (p->Error0 - 2.0f * p->Error1 + p->Error2);

    p->Out += delta;

    // 输出限幅（隐含了积分的限幅作用）
    if (p->Out > p->OutMax) p->Out = p->OutMax;
    if (p->Out < p->OutMin) p->Out = p->OutMin;

    return p->Out;
}
// PID_t motorPID;

// void main(void)
// {
//     // 初始化：Kp=2.5, Ki=0.1, Kd=0.05, 输出限幅 0~1000
//     PID_Init(&motorPID, 2.5f, 0.1f, 0.05f, 1000.0f, 0.0f);

//     while(1)
//     {
//         float currentSpeed = getMotorSpeed();
//         float targetSpeed  = 300.0f;

//         float output = PID_Positional_Update(&motorPID, currentSpeed, targetSpeed);
//         setMotorPWM(output);
//         delay(10);
//     }
// }