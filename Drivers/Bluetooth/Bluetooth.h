#ifndef DRIVERS_BLUETOOTH_BLUETOOTH_H_
#define DRIVERS_BLUETOOTH_BLUETOOTH_H_

#include <stdbool.h>
#include <stdint.h>

/* Must be called after SYSCFG_DL_init(). */
void BT_Init(void);

/*
 * Non-blocking transmit API. Data is copied into an internal queue.
 * false means the complete string was rejected because the queue is full.
 */
bool BT_SendStr(const char *str);

/*
 * Copies one CR/LF terminated command into buffer and removes it from the
 * receive queue. The terminator is not copied. Returns false if no complete
 * command is ready. capacity must include room for the trailing '\0'.
 */
bool BT_GetLine(char *buffer, uint16_t capacity);

#endif /* DRIVERS_BLUETOOTH_BLUETOOTH_H_ */
