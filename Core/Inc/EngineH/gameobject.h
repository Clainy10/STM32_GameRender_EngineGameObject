#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <stdint.h>
#include <stddef.h> /* NULL */
/* --- Sprite / Animation resource descriptors --- */
typedef struct {
    const uint16_t *pixels; /* RGB565 row-major (const in flash) */
    uint16_t w;
    uint16_t h;
} SpriteDef;

typedef struct {
    int spriteId;       /* index into engine sprite table */
    uint32_t duration;  /* ms */
} AnimFrame;

/* O animație stocată în engine */
typedef struct {
    const AnimFrame *frames; /* pointer la frame array (must be static/flash) */
    uint16_t count;
} AnimationDef;

/* forward */
struct GameObject;

/* logic callback prototype (per-object) */
typedef void (*LogicCb)(struct GameObject *o, uint32_t dt_ms);

/* background redraw callback: jocul re-desenază zona (x,y,w,h) */
typedef void (*BgRedrawCb)(int x, int y, int w, int h);

/* GameObject (pool element) */
typedef struct GameObject {
    uint8_t used;       /* 0/1 */
    int spriteId;       /* -1 = no visual */
    int animId;         /* -1 = no anim */
    uint32_t anim_time; /* ms into current frame */
    uint16_t anim_frame;/* current frame index */
    int x, y;           /* position (top-left) */
    int prev_x, prev_y; /* previous position (for erasing) */
    uint16_t w, h;      /* size (from sprite) */
    uint8_t visible;    /* 0/1 */
    uint8_t active;     /* 0/1 (logic enabled) */
    LogicCb logic;      /* per-object logic callback (can be NULL) */
    uint8_t z;          /* draw order */
} GameObject;

/* --- API exported by engine/gameobject module --- */

/* initialize pools */
void GameObject_Init(void);
void GameObject_SetBackgroundColor(uint16_t color);
/* sprite/animation resource creation (store pointers in engine arrays) */
int GameObject_CreateSprite_Solid(uint16_t w, uint16_t h, uint16_t color);
int GameObject_CreateSprite_Bitmap(uint16_t w, uint16_t h, const uint16_t *pixels);
int GameObject_CreateAnimation(const AnimFrame *frames, uint16_t frameCount);

/* spawn/destroy objects */
GameObject * GameObject_Spawn(int spriteId, int x, int y); /* spriteId can be -1 */
void GameObject_Destroy(GameObject *o);

/* transform/state helpers (use these so engine can handle redraw) */
void GameObject_SetPos(GameObject *o, int x, int y);
void GameObject_GetPos(GameObject *o, int *x, int *y);
void GameObject_SetVisible(GameObject *o, uint8_t visible);
void GameObject_SetZ(GameObject *o, uint8_t z);

/* component helpers */
void GameObject_SetSprite(GameObject *o, int spriteId);   /* -1 to remove */
void GameObject_PlayAnimation(GameObject *o, int animId); /* -1 to stop */
void GameObject_EnableLogic(GameObject *o, LogicCb cb);   /* pass NULL to disable */

/* called by engine each tick */
void GameObject_ProcessLogic(uint32_t dt_ms);
void GameObject_ProcessAnimation(uint32_t dt_ms);
void GameObject_RenderAll(void);

/* background redraw (game registers if it has tilemap) */
void GameObject_SetBgRedrawCallback(BgRedrawCb cb);

#endif /* GAMEOBJECT_H */
