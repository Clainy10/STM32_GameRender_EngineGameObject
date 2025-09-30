#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
/* include driver header so screen macros exist */
#include "ILI9341_STM32_Driver.h"
/* minimal renderer API used by engine */
void Render_Init(void);
void Render_ClearScreen(void);
int Render_EnqueueFillRect(int x,int y,int w,int h,uint16_t color, int z);
int Render_EnqueueBlitBitmap(const uint16_t *pixels,int w,int h,int x,int y, int z, int src_w);
void Render_Process(void);
/* background color helpers (used when engine erases old sprite area) */
void Render_SetBackgroundColor(uint16_t color);
uint16_t Render_GetBackgroundColor(void);
#endif
