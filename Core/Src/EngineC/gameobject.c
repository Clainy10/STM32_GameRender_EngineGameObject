#include "EngineH/gameobject.h"
#include "EngineH/render.h"
#include "stm32g0xx_hal.h"
#include <string.h> /* memset */

/* limits - ajustabile */
#define MAX_SPRITES  64
#define MAX_ANIMS    32
#define MAX_OBJECTS  128
/* background z for erase operations */
#define BG_Z_LAYER 0
/* internal sprite/anim storage */
typedef struct {
    uint8_t is_solid;  /* 1 = solid color, 0 = bitmap */
    uint16_t color;    /* valid if is_solid */
    const uint16_t *pixels; /* valid if bitmap */
    uint16_t w,h;
} _Sprite;

static _Sprite sprites[MAX_SPRITES];
static int sprite_count = 0;

typedef struct {
    const AnimFrame *frames;
    uint16_t count;
} _Anim;
static _Anim anims[MAX_ANIMS];
static int anim_count = 0;

/* objects pool (using type from header) */
static GameObject objects[MAX_OBJECTS];
static BgRedrawCb bg_cb = NULL;
static uint16_t redraw_bg_color = 0x0000;

void GameObject_SetBackgroundColor(uint16_t color){    redraw_bg_color = color;}

void GameObject_SetBgRedrawCallback(BgRedrawCb cb){ bg_cb = cb; }

void GameObject_Init(void){
    memset(sprites, 0, sizeof(sprites));
    sprite_count = 0;
    memset(anims, 0, sizeof(anims));
    anim_count = 0;
    memset(objects, 0, sizeof(objects));
}

/* Create sprite solid */
int GameObject_CreateSprite_Solid(uint16_t w, uint16_t h, uint16_t color){
    if(sprite_count >= MAX_SPRITES) return -1;
    sprites[sprite_count].is_solid = 1;
    sprites[sprite_count].color = color;
    sprites[sprite_count].pixels = NULL;
    sprites[sprite_count].w = w;
    sprites[sprite_count].h = h;
    return sprite_count++;
}

/* Create sprite bitmap */
int GameObject_CreateSprite_Bitmap(uint16_t w, uint16_t h, const uint16_t *pixels){
    if(sprite_count >= MAX_SPRITES) return -1;
    sprites[sprite_count].is_solid = 0;
    sprites[sprite_count].pixels = pixels;
    sprites[sprite_count].w = w;
    sprites[sprite_count].h = h;
    return sprite_count++;
}

/* Create animation (frames pointer must remain valid in flash/static) */
int GameObject_CreateAnimation(const AnimFrame *frames, uint16_t frameCount){
    if(anim_count >= MAX_ANIMS) return -1;
    anims[anim_count].frames = frames;
    anims[anim_count].count = frameCount;
    return anim_count++;
}

/* Spawn object */
GameObject * GameObject_Spawn(int spriteId, int x, int y){
    for(int i=0;i<MAX_OBJECTS;i++){
        if(!objects[i].used){
            GameObject *o = &objects[i];
            memset(o, 0, sizeof(GameObject)); /* clear all fields */
            o->used = 1;
            o->spriteId = spriteId;
            o->animId = -1;
            o->anim_time = 0;
            o->anim_frame = 0;
            o->x = x; o->y = y;
            o->prev_x = x; o->prev_y = y;
            o->visible = 1; o->active = 1; o->logic = NULL; o->z = 0;
            if(spriteId >= 0 && spriteId < sprite_count){
                o->w = sprites[spriteId].w;
                o->h = sprites[spriteId].h;
                /* initial draw (enqueue) */
                if(sprites[spriteId].is_solid){
                    Render_EnqueueFillRect(x, y, o->w, o->h, sprites[spriteId].color, o->z);
                } else {
                    Render_EnqueueBlitBitmap(sprites[spriteId].pixels, o->w, o->h, x, y, o->z, sprites[spriteId].w);
                }
            } else {
                o->w = o->h = 0;
            }
            return o;
        }
    }
    return NULL;
}

/* Destroy */
void GameObject_Destroy(GameObject *o){
    if(!o) return;
    if(o->used){
        /* erase previous area */
        if(bg_cb) bg_cb(o->prev_x, o->prev_y, o->w, o->h);
        else Render_EnqueueFillRect(o->prev_x, o->prev_y, o->w, o->h, redraw_bg_color, o->z);
        o->used = 0;
    }
}

/* Set sprite (handles erase + redraw) */
void GameObject_SetSprite(GameObject *o, int spriteId){
    if(!o) return;
    /* erase old */
    if(o->visible && o->w && o->h){
        if(bg_cb) bg_cb(o->prev_x, o->prev_y, o->w, o->h);
        else Render_EnqueueFillRect(o->prev_x, o->prev_y, o->w, o->h, redraw_bg_color, o->z);
    }
    o->spriteId = spriteId;
    if(spriteId >=0 && spriteId < sprite_count){
        o->w = sprites[spriteId].w;
        o->h = sprites[spriteId].h;
        /* draw new */
        if(sprites[spriteId].is_solid) Render_EnqueueFillRect(o->x, o->y, o->w, o->h, sprites[spriteId].color, o->z);
        else Render_EnqueueBlitBitmap(sprites[spriteId].pixels, o->w, o->h, o->x, o->y, o->z, sprites[spriteId].w);
    } else {
        o->w = o->h = 0;
    }
}

/* Set position: erase old, update pos, enqueue draw */
void GameObject_SetPos(GameObject *o, int x, int y){
    if(!o) return;
    if(o->x == x && o->y == y) return; // NU schimbăm nimic dacă poziția e la fel
    if(o->visible && o->w && o->h){
        if(bg_cb) bg_cb(o->prev_x, o->prev_y, o->w, o->h);
        else Render_EnqueueFillRect(o->prev_x, o->prev_y, o->w, o->h, redraw_bg_color, o->z);///////???????
    }
    o->prev_x = x; o->prev_y = y;
    o->x = x; o->y = y;
    if(o->spriteId >= 0 && o->spriteId < sprite_count){
        if(sprites[o->spriteId].is_solid) Render_EnqueueFillRect(o->x, o->y, o->w, o->h, sprites[o->spriteId].color, o->z);
        else Render_EnqueueBlitBitmap(sprites[o->spriteId].pixels, o->w, o->h, o->x, o->y, o->z, sprites[o->spriteId].w);
    }
}

void GameObject_GetPos(GameObject *o, int *x, int *y){ if(o && x && y){ *x = o->x; *y = o->y; } }
void GameObject_SetVisible(GameObject *o, uint8_t visible){ if(o) o->visible = visible?1:0; }
void GameObject_SetZ(GameObject *o, uint8_t z){ if(o) o->z = z; }

void GameObject_PlayAnimation(GameObject *o, int animId){
    if(!o) return;
    o->animId = animId;
    o->anim_time = 0;
    o->anim_frame = 0;
    if(animId >= 0 && animId < anim_count && anims[animId].count > 0){
        int spr = anims[animId].frames[0].spriteId;
        if(spr >= 0) GameObject_SetSprite(o, spr);
    }
}

void GameObject_EnableLogic(GameObject *o, LogicCb cb){
    if(!o) return;
    o->logic = cb;
}

/* iterate and call logic callbacks */
void GameObject_ProcessLogic(uint32_t dt_ms){
    for(int i=0;i<MAX_OBJECTS;i++){
        if(!objects[i].used) continue;
        if(objects[i].logic && objects[i].active){
            objects[i].logic(&objects[i], dt_ms);
        }
    }
}

/* process per-object animations */
void GameObject_ProcessAnimation(uint32_t dt_ms){
    for(int i=0;i<MAX_OBJECTS;i++){
        if(!objects[i].used) continue;
        GameObject *o = &objects[i];
        if(o->animId < 0 || o->animId >= anim_count) continue;
        _Anim *a = &anims[o->animId];
        if(a->count == 0) continue;
        o->anim_time += dt_ms;
        while(1){
            AnimFrame f = a->frames[o->anim_frame];
            if(o->anim_time < f.duration) break;
            o->anim_time -= f.duration;
            o->anim_frame++;
            if(o->anim_frame >= a->count) o->anim_frame = 0;
            f = a->frames[o->anim_frame];
            if(f.spriteId >= 0) GameObject_SetSprite(o, f.spriteId);
        }
    }
}

/* render: enqueue draws by z-order */
void GameObject_RenderAll(void){
    for(uint8_t z=0; z<255; ++z){
        for(int i=0;i<MAX_OBJECTS;i++){
            if(!objects[i].used) continue;
            GameObject *o = &objects[i];
            if(o->z != z) continue;
            if(!o->visible) continue;
            if(o->spriteId >= 0 && o->spriteId < sprite_count){
                if(sprites[o->spriteId].is_solid) Render_EnqueueFillRect(o->x, o->y, o->w, o->h, sprites[o->spriteId].color, o->z);
                else Render_EnqueueBlitBitmap(sprites[o->spriteId].pixels, o->w, o->h, o->x, o->y, o->z, sprites[o->spriteId].w);
            }
        }
    }
}
