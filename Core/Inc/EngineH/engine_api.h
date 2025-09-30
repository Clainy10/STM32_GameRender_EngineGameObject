#ifndef ENGINE_API_H
#define ENGINE_API_H

/* public API header that game files should include */
#include "EngineH/gameobject.h"
#include "EngineH/render.h"
#include "EngineH/input.h"
#include "fonts.h"
/* Game must provide these functions named Start/Update/Render (simple) */
extern void Start(void);
extern void Update(void);
extern void Render(void);

/* Engine functions (game can call if needed) */
void Engine_Init(void);
void Engine_RunLoop(void);
void Engine_SetLogicTick(uint32_t ms);
void Engine_SetRenderTick(uint32_t ms);

/* resource helpers (optional to expose) */
int CreateSprite_Bitmap(uint16_t w, uint16_t h, const uint16_t *pixels); /* returns spriteId */
int CreateSprite_Solid(uint16_t w, uint16_t h, uint16_t color);
int CreateAnimation(const AnimFrame *frames, uint16_t count);
GameObject *SpawnObject(int spriteId, int x, int y);
void DestroyObject(GameObject *o);
void SetObjectPos(GameObject *o, int x, int y);
void PlayObjectAnimation(GameObject *o, int animId);
void EnableObjectLogic(GameObject *o, void (*logic)(GameObject*, uint32_t));

#endif
