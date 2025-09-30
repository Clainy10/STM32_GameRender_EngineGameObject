#include "EngineH/render.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "stm32g0xx_hal.h"
#include <string.h>

#define MAX_QUEUE 64
typedef enum { CMD_FILL=0, CMD_BLIT } CmdType;

typedef struct {
    CmdType type;
    int x,y,w,h;
    int z;                  // pentru z-order
    uint16_t color;
    const uint16_t *pixels; // pointer la bitmap (row-major width = w_original)
    int src_w;              // original sprite width (useful for clipping offsets)
} Cmd;

static Cmd queue[MAX_QUEUE];
static int q_head=0, q_count=0;
static uint16_t bg_color = 0x0000;
static int clip_rect_to_screen(int *x,int *y,int *w,int *h,int *src_offset_x,int *src_offset_y,int src_w,int src_h);

void Render_Init(void){ q_head=0; q_count=0;}

void Render_SetBackgroundColor(uint16_t color){    bg_color = color;}

uint16_t Render_GetBackgroundColor(void){    return bg_color;}

void Render_ClearScreen(void){ ILI9341_FillScreen(bg_color); }

/* Enqueue fill rect with z */
int Render_EnqueueFillRect(int x,int y,int w,int h,uint16_t color,int z){
    if(q_count >= MAX_QUEUE) return -1;
    int idx = (q_head + q_count) % MAX_QUEUE;
    queue[idx].type   = CMD_FILL;
    queue[idx].x      = x;
    queue[idx].y      = y;
    queue[idx].w      = w;
    queue[idx].h      = h;
    queue[idx].color  = color;
    queue[idx].pixels = NULL;
    queue[idx].src_w = 0;
    queue[idx].z = z;
    q_count++;
    return 0;
}

/* Enqueue blit - src_w is sprite width in memory */
int Render_EnqueueBlitBitmap(const uint16_t *pixels,int w,int h,int x,int y,int z,int src_w){
    if(q_count >= MAX_QUEUE) return -1;
    int idx = (q_head + q_count) % MAX_QUEUE;
    queue[idx].type = CMD_BLIT;
    queue[idx].x = x; queue[idx].y = y;
    queue[idx].w = w; queue[idx].h = h;
    queue[idx].z = z;
    queue[idx].pixels = pixels;
    queue[idx].src_w = src_w ? src_w : w; /* if src_w==0 assume tight */
    q_count++;
    return 0;
}


/* Process queue: copy to temp linear array, sort by z asc, then draw. */
void Render_Process(void){
    if(q_count == 0) return;

    /* 1) copy to linear array */
    Cmd list[MAX_QUEUE];
    int n = q_count;
    for(int i=0;i<n;i++){
        int idx = (q_head + i) % MAX_QUEUE;
        list[i] = queue[idx];
    }
    /* reset queue quickly */
    q_head = 0; q_count = 0;

    /* 2) sort by z (ascending) - simple stable sort */
    for(int i=0;i<n-1;i++){
        for(int j=i+1;j<n;j++){
            if(list[i].z > list[j].z){
                Cmd tmp = list[i];
                list[i] = list[j];
                list[j] = tmp;
            }
        }
    }

    /* 3) process in order */
    for(int ci=0; ci<n; ci++){
        Cmd *c = &list[ci];

        if(c->type == CMD_FILL){
            /* clip */
            int x = c->x, y = c->y, w = c->w, h = c->h;
            int sox=0, soy=0;
            if(!clip_rect_to_screen(&x,&y,&w,&h,&sox,&soy,0,0)) continue;
            ILI9341_DrawRectangle(x,y,w,h,c->color);
            continue;
        }

        /* c->type == CMD_BLIT */
        if(c->pixels == NULL) continue;

        /* clip sprite area and compute src offset */
        int x = c->x, y = c->y, w = c->w, h = c->h;
        int src_off_x = 0, src_off_y = 0;
        if(!clip_rect_to_screen(&x,&y,&w,&h,&src_off_x,&src_off_y,c->src_w,c->h)) continue;

        const uint16_t *pixels = c->pixels;
        /* start pointer into source pixels: row = src_off_y, col = src_off_x */
        pixels += (src_off_y * c->src_w) + src_off_x;

            /* contiguous block if we clipped whole rect partially: but rows in source may have stride src_w */
            /* We must write row by row because pixels are arranged with src_w stride. */
            for(int row = 0; row < h; row++){
                const uint16_t *row_ptr = pixels + row * c->src_w;
                ILI9341_SetAddress((uint16_t)x, (uint16_t)(y + row), (uint16_t)(x + w - 1), (uint16_t)(y + row));
                /* write contiguous row of w pixels */
                ILI9341_WriteBuffer((uint8_t*)row_ptr, (uint16_t)(w * 2));
            }
    	}
}

/* helper: clip rectangle to screen. Returns 0 if empty, else returns clipped x,y,w,h and updates src offsets */
static int clip_rect_to_screen(int *x,int *y,int *w,int *h,int *src_offset_x,int *src_offset_y,int src_w,int src_h){
    int sx = *x, sy = *y, sw = *w, sh = *h;
    int offx = 0, offy = 0;

    if(sx < 0){
        offx = -sx;
        sw += sx; // reduce width
        sx = 0;
    }
    if(sy < 0){
        offy = -sy;
        sh += sy;
        sy = 0;
    }
    if(sx + sw > (int)ILI9341_SCREEN_WIDTH) sw = ILI9341_SCREEN_WIDTH - sx;
    if(sy + sh > (int)ILI9341_SCREEN_HEIGHT) sh = ILI9341_SCREEN_HEIGHT - sy;

    if(sw <= 0 || sh <= 0) return 0;

    *x = sx; *y = sy; *w = sw; *h = sh;
    *src_offset_x = offx;
    *src_offset_y = offy;
    return 1;
}




