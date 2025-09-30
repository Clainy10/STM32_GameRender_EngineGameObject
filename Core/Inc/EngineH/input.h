#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

typedef enum { DIR_NONE=0, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

void Input_Init(void);
void Input_Poll(void);
Direction Input_ConsumeEvent(void);
uint8_t Input_IsHeld(Direction d);

#endif
