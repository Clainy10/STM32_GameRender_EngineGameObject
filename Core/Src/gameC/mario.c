/* mario.c - World 1-1 (playable lite) for your engine
   Put this file in Core/Src/gameC and the header in Core/Inc/gameH
*/

#include "EngineH/engine_api.h"
#include "gameH/mario.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include <stdio.h>
#include <string.h>

/* ----- gameplay tuning ----- */
#define GRAVITY_PIX    1
#define MAX_FALL_V     10
#define MOVE_SPEED     2
#define JUMP_V         -14
#define PLAYER_W       16
#define PLAYER_H       16
#define GOOMBA_W       16
#define GOOMBA_H       12

#ifndef BUTTON_JUMP
  #define BUTTON_JUMP 0
#endif

#ifndef RGB565
  #define RGB565(r,g,b) (uint16_t)((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))
#endif

/* sprites/anim ids */
static int spr_player_idle = -1;
static int spr_player_run_a = -1;
static int spr_player_run_b = -1;
static int anim_player_run = -1;
static int spr_goomba = -1;
static int spr_coin = -1;

/* player state */
static GameObject *player = NULL;
static int player_vx = 0;
static int player_vy = 0;
static int player_on_ground = 0;
static int start_x = 10, start_y = 0; /* start_y set in Start() */

/* score / HUD */
static int score = 0;
static int score_dirty = 1; /* true -> update HUD */

/* enemies */
#define MAX_ENEMIES 8
typedef struct { GameObject *o; int vx; } Enemy;
static Enemy enemies[MAX_ENEMIES];

/* world blocks */
#define MAX_BLOCKS 32
typedef struct { int x,y,w,h; } Block;
static Block blocks[MAX_BLOCKS];
static int block_count = 0;

/* coins */
#define MAX_COINS 16
static GameObject *coins[MAX_COINS];
static int coin_count = 0;

/* util */
static int rects_overlap(int ax,int ay,int aw,int ah, int bx,int by,int bw,int bh){
    if(ax + aw <= bx) return 0;
    if(bx + bw <= ax) return 0;
    if(ay + ah <= by) return 0;
    if(by + bh <= ay) return 0;
    return 1;
}

/* Create solid colored sprite convenience */
static int CreateSolid(int w,int h,uint16_t color){
    return GameObject_CreateSprite_Solid(w,h,color);
}

/* blocks list and GameObjects creation */
static void AddBlock(int x,int y,int w,int h,uint16_t color){
    if(block_count >= MAX_BLOCKS) return;
    blocks[block_count].x = x; blocks[block_count].y = y;
    blocks[block_count].w = w; blocks[block_count].h = h;
    block_count++;

    int sid = CreateSolid(w,h,color);
    GameObject *b = GameObject_Spawn(sid, x, y);
    if(b){ b->visible = 1; b->active = 0; b->z = 0; }
}

/* spawn goomba - stores in enemies[] */
static void SpawnGoomba(int x,int y,int vx){
    for(int i=0;i<MAX_ENEMIES;i++){
        if(enemies[i].o == NULL){
            GameObject *g = GameObject_Spawn(spr_goomba, x, y);
            if(g){
                g->visible = 1; g->active = 1; g->z = 1;
                GameObject_EnableLogic(g, NULL); /* set logic below */
                /* attach GoombaLogic using engine API if available; we will set active logic pointer:
                   Assume engine offers GameObject_EnableLogic(g, func). Use engine function directly. */
                GameObject_EnableLogic(g, NULL); /* placeholder if engine requires call before pointer set */
                enemies[i].o = g;
                enemies[i].vx = vx;
            }
            return;
        }
    }
}

/* we will implement Goomba logic as a separate function and enable it after spawn */
static void GoombaLogic(GameObject *o, uint32_t dt_ms){
    (void)dt_ms;
    Enemy *en = NULL;
    for(int i=0;i<MAX_ENEMIES;i++){
        if(enemies[i].o == o){ en = &enemies[i]; break; }
    }
    if(!en) return;
    int curx = o->x;
    int cury = o->y;
    int nx = curx + en->vx;

    /* horizontal collision with blocks -> reverse */
    int collided = 0;
    for(int i=0;i<block_count;i++){
        if(rects_overlap(nx, cury, GOOMBA_W, GOOMBA_H,
                         blocks[i].x, blocks[i].y, blocks[i].w, blocks[i].h)){
            collided = 1; break;
        }
    }
    if(collided){
        en->vx = -en->vx;
    } else {
        GameObject_SetPos(o, nx, cury);
        curx = nx;
    }

    /* basic gravity */
    int on_ground = 0;
    for(int i=0;i<block_count;i++){
        if(rects_overlap(curx, cury+1, GOOMBA_W, GOOMBA_H,
                         blocks[i].x, blocks[i].y, blocks[i].w, blocks[i].h)){
            on_ground = 1; break;
        }
    }
    if(!on_ground){
        GameObject_SetPos(o, curx, cury+1);
    }
}

/* Player logic uses setter for every move to avoid artifacts */
static void PlayerLogic(GameObject *o, uint32_t dt_ms){
    (void)dt_ms;
    int curx = o->x;
    int cury = o->y;

    int left = Input_IsHeld(DIR_LEFT);
    int right = Input_IsHeld(DIR_RIGHT);
    int up = Input_IsHeld(DIR_UP) || Input_IsHeld(BUTTON_JUMP);

    if(left && !right) player_vx = -MOVE_SPEED;
    else if(right && !left) player_vx = MOVE_SPEED;
    else player_vx = 0;

    if(up && player_on_ground){
        player_vy = JUMP_V;
        player_on_ground = 0;
    }

    /* gravity */
    player_vy += GRAVITY_PIX;
    if(player_vy > MAX_FALL_V) player_vy = MAX_FALL_V;

    int nx = curx + player_vx;
    int ny = cury + player_vy;

    /* horizontal collision */
    int blockedX = 0;
    for(int i=0;i<block_count;i++){
        if(rects_overlap(nx, cury, PLAYER_W, PLAYER_H,
                         blocks[i].x, blocks[i].y, blocks[i].w, blocks[i].h)){
            blockedX = 1; break;
        }
    }
    if(!blockedX){
        GameObject_SetPos(o, nx, cury);
        curx = nx;
    }

    /* vertical collision */
    int blockedY = 0;
    player_on_ground = 0;
    for(int i=0;i<block_count;i++){
        if(rects_overlap(curx, ny, PLAYER_W, PLAYER_H,
                         blocks[i].x, blocks[i].y, blocks[i].w, blocks[i].h)){
            blockedY = 1;
            if(player_vy > 0){
                /* land on top */
                GameObject_SetPos(o, curx, blocks[i].y - PLAYER_H);
                player_vy = 0;
                player_on_ground = 1;
            } else {
                /* hit head */
                GameObject_SetPos(o, curx, blocks[i].y + blocks[i].h);
                player_vy = 0;
            }
            break;
        }
    }
    if(!blockedY){
        GameObject_SetPos(o, curx, ny);
    }

    /* coins collision */
    for(int i=0;i<coin_count;i++){
        GameObject *c = coins[i];
        if(c && c->visible){
            if(rects_overlap(o->x, o->y, PLAYER_W, PLAYER_H, c->x, c->y, 8, 8)){
                /* collect */
                c->visible = 0;
                c->active = 0;
                score += 10;
                score_dirty = 1;
            }
        }
    }

    /* enemy collisions */
    for(int i=0;i<MAX_ENEMIES;i++){
        if(enemies[i].o){
            GameObject *e = enemies[i].o;
            if(rects_overlap(o->x, o->y, PLAYER_W, PLAYER_H, e->x, e->y, GOOMBA_W, GOOMBA_H)){
                /* if falling onto enemy -> stomp */
                if(player_vy > 0 && (o->y + PLAYER_H - e->y) < 12){
                    /* remove enemy */
                    e->visible = 0; e->active = 0;
                    enemies[i].o = NULL; /* free slot */
                    player_vy = JUMP_V/2;
                    score += 100;
                    score_dirty = 1;
                } else {
                    /* player hurt -> respawn */
                    GameObject_SetPos(o, start_x, start_y);
                    player_vx = player_vy = 0;
                    player_on_ground = 0;
                }
            }
        }
    }

    /* animation control */
    if(player_vx != 0){
        if(o->animId != anim_player_run) GameObject_PlayAnimation(o, anim_player_run);
    } else {
        if(o->animId != -1) GameObject_PlayAnimation(o, -1);
        if(o->spriteId != spr_player_idle) GameObject_SetSprite(o, spr_player_idle);
    }

    /* fall off world -> respawn */
    if(o->y > ILI9341_SCREEN_HEIGHT + 64){
        GameObject_SetPos(o, start_x, start_y);
        player_vx = player_vy = 0;
        player_on_ground = 0;
    }
}

/* Level builder */
static void CreateLevel_World1_1(void){
    block_count = 0;
    score = 0;
    coin_count = 0;

    int ground_y = ILI9341_SCREEN_HEIGHT - 16;
    AddBlock(0, ground_y, ILI9341_SCREEN_WIDTH, 16, RGB565(100,50,10));

    AddBlock(60, ground_y - 32, 32, 16, RGB565(180,180,50));
    AddBlock(110, ground_y - 64, 32, 16, RGB565(180,180,50));
    AddBlock(180, ground_y - 48, 48, 16, RGB565(180,180,50));

    AddBlock(260, ground_y - 16, 16, 16, RGB565(100,200,100));
    AddBlock(260, ground_y - 32, 16, 16, RGB565(100,200,100));
    AddBlock(260, ground_y - 48, 16, 16, RGB565(100,200,100));

    AddBlock(ILI9341_SCREEN_WIDTH - 40, ground_y - 32, 40, 32, RGB565(40,160,40));

    /* spawn goombas */
    SpawnGoomba(120, ground_y - GOOMBA_H, -1);
    SpawnGoomba(200, ground_y - GOOMBA_H, 1);

    /* coins */
    spr_coin = CreateSolid(8,8, RGB565(255,200,0));
    GameObject *c1 = GameObject_Spawn(spr_coin, 65, ground_y - 32 - 12);
    if(c1){ c1->visible = 1; c1->active = 0; c1->z = 2; if(coin_count < MAX_COINS) coins[coin_count++] = c1; }
    GameObject *c2 = GameObject_Spawn(spr_coin, 115, ground_y - 64 - 12);
    if(c2){ c2->visible = 1; c2->active = 0; c2->z = 2; if(coin_count < MAX_COINS) coins[coin_count++] = c2; }
}

/* HUD update (draw small text) - called when score_dirty */
static void DrawHUD(void){
    char buf[32];
    sprintf(buf, "SCORE: %04d", score);
    /* draw over small fixed area top-left (background fill + text) */
    ILI9341_DrawFilledRectangleCoord(0, 0, 120, 24, RGB565(70,170,255));
    ILI9341_DrawText(buf, FONT3, 4, 4, WHITE, RGB565(70,170,255));
    score_dirty = 0;
}

/* Engine callbacks */
void Start(void){
    /* background */
    Render_SetBackgroundColor(RGB565(70,170,255));
    GameObject_SetBackgroundColor(RGB565(70,170,255));
    Render_ClearScreen();

    /* create player sprites & animation */
    spr_player_idle = CreateSolid(PLAYER_W, PLAYER_H, RGB565(255, 48, 48));
    spr_player_run_a = CreateSolid(PLAYER_W, PLAYER_H, RGB565(255, 128, 48));
    spr_player_run_b = CreateSolid(PLAYER_W, PLAYER_H, RGB565(255, 200, 48));
    static AnimFrame f[2];
    f[0].spriteId = spr_player_run_a; f[0].duration = 120;
    f[1].spriteId = spr_player_run_b; f[1].duration = 120;
    anim_player_run = GameObject_CreateAnimation(f, 2);

    spr_goomba = CreateSolid(GOOMBA_W, GOOMBA_H, RGB565(160,90,30));

    /* clear arrays */
    for(int i=0;i<MAX_ENEMIES;i++){ enemies[i].o = NULL; enemies[i].vx = 0; }
    for(int i=0;i<MAX_COINS;i++) coins[i] = NULL;

    /* build level and spawn player */
    CreateLevel_World1_1();
    start_y = ILI9341_SCREEN_HEIGHT - 64;
    player = GameObject_Spawn(spr_player_idle, start_x, start_y);
    if(player){
        player->visible = 1; player->active = 1; player->z = 5;
        GameObject_EnableLogic(player, PlayerLogic);
        player_vx = player_vy = 0; player_on_ground = 0;
    }

    /* enable goomba logic for spawned enemies */
    for(int i=0;i<MAX_ENEMIES;i++){
        if(enemies[i].o){
            GameObject_EnableLogic(enemies[i].o, GoombaLogic);
        }
    }

    score = 0;
    score_dirty = 1;
    DrawHUD();
}

void Update(void){
    /* nothing heavy here - keep light */
    /* OPTIONAL: spawn new enemies or handle timers */
}

void Render(void){
    /* Keep this function cheap. We only update HUD when needed. */
    if(score_dirty) DrawHUD();
}
