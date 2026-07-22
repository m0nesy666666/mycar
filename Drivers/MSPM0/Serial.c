#include "Serial.h"
#include <stdarg.h>
#include <string.h>
//串口收发.hex数据包
uint8_t Serial_TxPacket[4];				// 定义发送数据包数组，格式：FF 01 02 03 04 FE
uint8_t Serial_RxPacket[4];				// 定义接收数据包数组
volatile uint8_t Serial_RxFlag=0;					// 定义接收数据包标志位

/**
  * 函    数：串口发送一个字节
  */
void Serial_SendByte(uint8_t Byte)
{
    // 调用 TI 官方库，自带阻塞等待 TX 缓冲区清空的功能
    DL_UART_Main_transmitDataBlocking(UART_DEBUG_INST, Byte);
}

/**
  * 函    数：串口发送一个数组
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

/**
  * 函    数：串口发送一个字符串
  */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

/**
  * 函    数：次方函数（内部使用）
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

/**
  * 函    数：串口发送数字
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * 重定向 printf 的底层函数 (适配 TI 编译器与 GCC)
  */
int fputc(int c, FILE* stream)
{
	DL_UART_Main_transmitDataBlocking(UART_DEBUG_INST, c);
	return c;
}

int fputs(const char* restrict s, FILE* restrict stream)
{
	uint16_t i, len;
	len = strlen(s);
	for(i=0; i<len; i++)
	{
		DL_UART_Main_transmitDataBlocking(UART_DEBUG_INST, s[i]);
	}
	return len;
}

int puts(const char* _ptr)
{
	int count = fputs(_ptr, stdout);
	count += fputs("\n", stdout);
	return count;
}

/**
  * 函    数：自己封装的prinf函数
  */
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

/**
  * 函    数：串口发送数据包 (打包 FF 和 FE)
  */
void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket, 4);//*** */
	Serial_SendByte(0xFE);
}

/**
  * 函    数：获取串口接收数据包标志位
  */
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

/**
  * 函    数：
  * 说    明：使用状态机接收 FF [4字节数据] FE 格式的数据包
  */
// void UART_DEBUG_INST_IRQHandler (void)
// {
// 	static uint8_t RxState = 0;		// 状态机
// 	static uint8_t pRxPacket = 0;	// 数据索引
// 	uint8_t RxData;
	
//     // 检查是否是 RX 接收中断触发
//     switch (DL_UART_Main_getPendingInterrupt(UART_DEBUG_INST)) {
//         case DL_UART_MAIN_IIDX_RX:
//             RxData = DL_UART_Main_receiveData(UART_DEBUG_INST); // 读取接收到的数据

//             /* 状态机解析  */
//             if (RxState == 0)
//             {
//                 if (RxData == 0xFF)
//                 {
//                     RxState = 1;
//                     pRxPacket = 0;
//                 }
//             }
//             else if (RxState == 1)
//             {
//                 Serial_RxPacket[pRxPacket] = RxData;
//                 pRxPacket++;
//                 if (pRxPacket >= 4)
//                 {
//                     RxState = 2;
//                 }
//             }
//             else if (RxState == 2)
//             {
//                 if (RxData == 0xFE)
//                 {
//                     RxState = 0;
//                     Serial_RxFlag = 1; // 接收完成标志位置1
//                 }
//                 else
//                 {
//                     RxState = 0; // 如果包尾不是 FE，说明传输出错，状态机复位
//                 }
//             }
//             break;
            
//         default:
//             break;
//     }
// }
char Serial_RxString[32];               // 定义接收字符串缓冲区
// uint8_t Serial_RxFlag = 0;              // 定义接收标志位



/**
 * 函    数：UART 中断服务函数 (改为接收字符串，以 \n 或 \r 结尾).VOFA调参
 */
void UART_DEBUG_INST_IRQHandler(void)
{
    static uint8_t pRxPacket = 0; 
    
    switch (DL_UART_Main_getPendingInterrupt(UART_DEBUG_INST)) {
        case DL_UART_MAIN_IIDX_RX:
        {
            char RxData = DL_UART_Main_receiveData(UART_DEBUG_INST); 

            // 如果收到换行符或回车符，说明一帧字符串结束
            if (RxData == '\n' || RxData == '\r') 
            {
                if (pRxPacket > 0) // 防止连续的 \r\n 触发空包
                {
                    Serial_RxString[pRxPacket] = '\0'; // 加上字符串结束符
                    Serial_RxFlag = 1;                 // 置位标志
                    pRxPacket = 0;                     // 索引清零，准备下一次接收
                }
            } 
            else 
            {
                if (pRxPacket < 31) // 防止数组越界
                {
                    Serial_RxString[pRxPacket] = RxData;
                    pRxPacket++;
                }
            }
            break;
        }
        default:
            break;
    }
}

// void UART_DEBUG_INST_IRQHandler(void)
// {
//     /*
//      * RxState = 0：等待帧头 '['
//      * RxState = 1：接收数据内容
//      * RxState = 2：已经收到 '*'，等待帧尾 ']'
//      */
//     static uint8_t RxState = 0;
//     static uint8_t pRxPacket = 0;

//     char RxData;

//     switch (DL_UART_Main_getPendingInterrupt(UART_DEBUG_INST))
//     {
//         case DL_UART_MAIN_IIDX_RX:
//         {
//             RxData = (char)DL_UART_Main_receiveData(UART_DEBUG_INST);

//             /* 主循环还没有处理上一帧，暂时不接收下一帧 */
//             if (Serial_RxFlag == 1)
//             {
//                 break;
//             }

//             /* 状态0：等待帧头 '[' */
//             if (RxState == 0)
//             {
//                 if (RxData == '[')
//                 {
//                     pRxPacket = 0;
//                     RxState = 1;
//                 }
//             }

//             /* 状态1：接收 x,y 数据 */
//             else if (RxState == 1)
//             {
//                 /* 收到数据结束标志 '*' */
//                 if (RxData == '*')
//                 {
//                     Serial_RxString[pRxPacket] = '\0';
//                     RxState = 2;
//                 }

//                 /* 接收过程中又收到 '['，重新开始一帧 */
//                 else if (RxData == '[')
//                 {
//                     pRxPacket = 0;
//                 }

//                 /* 接收普通字符，例如 11,-10 */
//                 else
//                 {
//                     if (pRxPacket < sizeof(Serial_RxString) - 1)
//                     {
//                         Serial_RxString[pRxPacket] = RxData;
//                         pRxPacket++;
//                     }
//                     else
//                     {
//                         /* 数据超长，丢弃当前帧 */
//                         pRxPacket = 0;
//                         RxState = 0;
//                     }
//                 }
//             }

//             /* 状态2：收到 '*' 后等待帧尾 ']' */
//             else if (RxState == 2)
//             {
//                 if (RxData == ']')
//                 {
//                     /* 完整收到 [x,y*] */
//                     Serial_RxFlag = 1;
//                 }

//                 /* 无论是否收到正确帧尾，都重新等待下一帧 */
//                 pRxPacket = 0;
//                 RxState = 0;
//             }

//             break;
//         }

//         default:
//             break;
//     }
// }
