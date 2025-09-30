#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include "EngineH/gameobject.h"  // <- doar includem, nu redefinim structurile

/* Game structure (user creates one per game) */
typedef struct {
    void (*Init)(void);    /* Start() */
    void (*Update)(void);  /* Update() called at logic tick */
    void (*Render)(void);  /* optional per-frame render (engine also draws GameObjects) */
} Game;

/* Register game (call before Engine_Init) */
void Engine_RegisterGame(const Game *g);

/* Engine init & runloop control */
void Engine_Init(void);

/* Set ticks in milliseconds */
void Engine_SetLogicTick(uint32_t ms);
void Engine_SetRenderTick(uint32_t ms);

/* Main runloop (blocks) */
void Engine_RunLoop(void);

#endif
