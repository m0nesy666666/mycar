#ifndef __SERIAL_H
#define __SERIAL_H

#include "ti_msp_dl_config.h"
#include <stdio.h>
#include <stdint.h>

extern uint8_t Serial_TxPacket[4];
extern uint8_t Serial_RxPacket[4];
extern uint8_t Serial_RxFlag;
extern char Serial_RxString[32];   

void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

void Serial_SendPacket(void);
uint8_t Serial_GetRxFlag(void);

#endif
