/* Core/Src/EngineC/engine.c */
#include "EngineH/engine_api.h"   /* public API / game entry points */
#include "EngineH/gameobject.h"
#include "EngineH/render.h"
#include "EngineH/input.h"
#include <string.h> /* memset */
#include "stm32g0xx_hal.h"

static uint32_t logic_tick_ms = 50;   /* default 20 Hz */
static uint32_t render_tick_ms = 33;  /* default ~30 Hz */

void Engine_SetLogicTick(uint32_t ms){ if(ms) logic_tick_ms = ms; }
void Engine_SetRenderTick(uint32_t ms){ if(ms) render_tick_ms = ms; }

void Engine_Init(void)
{
    /* init subsisteme (TFT driver must be initialized before, from main) */
    Render_Init();
    Input_Init();
    GameObject_Init();

    /* call user Start() to create sprites/objects/etc. */
    Start();
}

void Engine_RunLoop(void)
{
    uint32_t last_logic = HAL_GetTick();
    uint32_t last_render = HAL_GetTick();

    while(1)
    {
        /* 1) Poll input as often as possible */
        Input_Poll();

        uint32_t now = HAL_GetTick();

        /* 2) Logic tick */
        if((now - last_logic) >= logic_tick_ms)
        {
            uint32_t dt = now - last_logic;

            /* user update */
            Update();

            /* per-object logic & animation processing */
            GameObject_ProcessLogic(dt);
            GameObject_ProcessAnimation(dt);

            last_logic = now;
        }

        /* 3) Render tick */
        if((now - last_render) >= render_tick_ms)
        {
            /* optional user render hook (background, UI) */
            Render();

            /* queue render of engine-managed objects */
           GameObject_RenderAll();

            /* flush renderer queue to display */
            Render_Process();

            last_render = now;
        }

        /* small sleep to yield CPU (keep it small) */
        HAL_Delay(1);
    }
}
