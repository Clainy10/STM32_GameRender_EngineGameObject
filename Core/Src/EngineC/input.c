#include "EngineH/input.h"
#include "stm32g0xx_hal.h"

/* --- Configure your pins here (match CubeMX) --- */
#ifndef BTN_UP_PORT
#define BTN_UP_PORT    GPIOA
#define BTN_UP_PIN     GPIO_PIN_0
#define BTN_DOWN_PORT  GPIOA
#define BTN_DOWN_PIN   GPIO_PIN_1
#define BTN_LEFT_PORT  GPIOA
#define BTN_LEFT_PIN   GPIO_PIN_4
#define BTN_RIGHT_PORT GPIOB
#define BTN_RIGHT_PIN  GPIO_PIN_1
#endif

/* set 1 if using internal pull-up and buttons connect to GND (pressed -> 0) */
#ifndef BUTTON_ACTIVE_LOW
#define BUTTON_ACTIVE_LOW 1
#endif

/* debounce time */
#define DEBOUNCE_MS 30

static volatile Direction latest_event = DIR_NONE;

/* last stable states and times */
static uint8_t last_up=0, last_down=0, last_left=0, last_right=0;
static uint32_t t_up=0, t_down=0, t_left=0, t_right=0;

/* reads physical pin, returns 1 if pressed */
static inline uint8_t read_pin(GPIO_TypeDef* port, uint16_t pin){
    GPIO_PinState s = HAL_GPIO_ReadPin(port, pin);
#if BUTTON_ACTIVE_LOW
    return (s == GPIO_PIN_RESET) ? 1 : 0;
#else
    return (s == GPIO_PIN_SET) ? 1 : 0;
#endif
}

void Input_Init(void){
    last_up = last_down = last_left = last_right = 0;
    t_up = t_down = t_left = t_right = HAL_GetTick();
    latest_event = DIR_NONE;
}

void Input_Poll(void){
    uint32_t now = HAL_GetTick();
    uint8_t cur;

    /* UP */
    cur = read_pin(BTN_UP_PORT, BTN_UP_PIN);
    if(cur && !last_up && (now - t_up) > DEBOUNCE_MS){
        latest_event = DIR_UP;
        last_up = 1; t_up = now;
    }
    if(!cur && last_up){ last_up = 0; t_up = now; }

    /* DOWN */
    cur = read_pin(BTN_DOWN_PORT, BTN_DOWN_PIN);
    if(cur && !last_down && (now - t_down) > DEBOUNCE_MS){
        latest_event = DIR_DOWN;
        last_down = 1; t_down = now;
    }
    if(!cur && last_down){ last_down = 0; t_down = now; }

    /* LEFT */
    cur = read_pin(BTN_LEFT_PORT, BTN_LEFT_PIN);
    if(cur && !last_left && (now - t_left) > DEBOUNCE_MS){
        latest_event = DIR_LEFT;
        last_left = 1; t_left = now;
    }
    if(!cur && last_left){ last_left = 0; t_left = now; }

    /* RIGHT */
    cur = read_pin(BTN_RIGHT_PORT, BTN_RIGHT_PIN);
    if(cur && !last_right && (now - t_right) > DEBOUNCE_MS){
        latest_event = DIR_RIGHT;
        last_right = 1; t_right = now;
    }
    if(!cur && last_right){ last_right = 0; t_right = now; }
}

/* consume last edge event */
Direction Input_ConsumeEvent(void){
    Direction d = latest_event;
    latest_event = DIR_NONE;
    return d;
}

/* hold detection (useful for continuous movement) */
uint8_t Input_IsHeld(Direction d){
    switch(d){
        case DIR_UP: return last_up;
        case DIR_DOWN: return last_down;
        case DIR_LEFT: return last_left;
        case DIR_RIGHT: return last_right;
        default: return 0;
    }
}
