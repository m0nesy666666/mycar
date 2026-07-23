// #include "ti_msp_dl_config.h"
// #include "GRAY.h"

// /*灰度循迹模块检测到黑线是0，但是后边为了方便数据存储时改成黑色是1*/

// /* 灰度模块内部数据：1表示检测到黑线，0表示未检测到黑线。 */
// static int S[8];

// //读取误差值
// void Gray_Read(void)
// {
// S[0] = (DL_GPIO_readPins(GPIOA, GPIOA_OUT1_PA27_PIN) != 0U);//黑线是1 白线是0 
// S[1] = (DL_GPIO_readPins(GPIOA, GPIOA_OUT2_PA25_PIN) != 0U);
// S[2] = (DL_GPIO_readPins(GPIOB, GPIOB_OUT3_PB25_PIN) != 0U);
// S[3] = (DL_GPIO_readPins(GPIOB, GPIOB_OUT4_PB20_PIN) != 0U);
// S[4] = (DL_GPIO_readPins(GPIOA, GPIOA_OUT5_PA14_PIN) != 0U);
// S[5] = (DL_GPIO_readPins(GPIOA, GPIOA_OUT6_PA16_PIN) != 0U);
// S[6] = (DL_GPIO_readPins(GPIOA, GPIOA_OUT7_PA17_PIN) != 0U);
// S[7] = (DL_GPIO_readPins(GPIOA, GPIOA_OUT8_PA15_PIN) != 0U);
// }

// /*
//  * 将S[0]~S[7]转换为"00111100"形式的字符串。
//     显示和蓝牙发送专用
// */
// void Gray_GetText(char text[9])
// {
//     unsigned char i;
//     for (i = 0U; i < 8U; i++) {
//         text[i] = S[i] ? '1' : '0';
//     }
//     text[8] = '\0';
// }


// int Gray_HasLine(void)
// {
//     unsigned char i;

//     for (i = 0U; i < 8U; i++) {
//         if (S[i] != 0) {
//             return 1;
//         }
//     }

//     return 0;
// }

// float weight[8] = {-7.0f,-5.0f,-3.0f,-1.0f,1.0f,3.0f,5.0f,7.0f};//越靠外侧误差增长越大

// //计算误差
// float GetLineError(void)
// {
//     float sum = 0.0f;
//     int cnt = 0;
//     unsigned char i;
//     float error;//全局变量，只是读取当前误差不会突变，清零也没事
//     static float last_err = 0.0f;//静态变量，每次访问误差不会清零

//     for(i = 0;i<8;i++)
//     {
//         sum += weight[i] * S[i];
//         cnt += S[i];
//     }
//     if (cnt!=0)//cnt>=0,不全是0，也就是不全为白地，就要巡线 
//     {
//         error = (float)sum /cnt;//强制转换成浮点数
//         last_err = error;//更新上次误差
//     }
//     else
//     {
//         error = last_err; //丢线时用上一次方向
//     }
//     return error;
// }

