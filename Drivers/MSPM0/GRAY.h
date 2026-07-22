#ifndef GRAY_H
#define GRAY_H

float GetLineError(void);
void Gray_Read(void);
int Gray_HasLine(void);

/* 将当前八路灰度状态转换为8个字符，text至少需要9字节。 */
void Gray_GetText(char text[9]);

#endif
