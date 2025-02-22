#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Util.h"

#include "player.h"

static void tick       (entity_t *self);
static void draw       (entity_t *self);
static void move       (entity_t *self);
static void die        (entity_t *self);
static void do_hitbox  (entity_t *self);
static int  is_control (int type);

static int            facing;
static atlas_image_t *player_texture;

extern app_t   app;
extern game_t  game;
extern stage_t stage;

void init_player(entity_t *e)
{
    /* Set textures */
    player_texture = IDG_GetAtlasImage("data/gfx/tiles/tile2.png", 1);

    /* Set mallocs */

    /* Set dimensions */

    e->texture  = player_texture;
    e->flags    = EF_WEIGHTLESS;
    e->friction = P_FRICTION;

    e->tick     = tick;
    e->draw     = draw;
    e->die      = die;

    stage.player = e;
}

static void tick(entity_t *self) {
    move      (self);
    do_hitbox (self);
}

static void draw(entity_t *self) 
{
    IDG_BlitAtlasImage(
        self->texture,
        (self->x-stage.camera.pos.x), (self->y-stage.camera.pos.y),
        0,
        self->facing = FACING_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
    );
}

static void move(entity_t *self) {
    // self->dx = self->dy = 0;
    
    // TODO - why doesnt is_control(...) work?

    if(app.keyboard[SDL_SCANCODE_A])
    {
        self->dx     = -P_RUN_SPEED;
        self->facing =  FACING_LEFT;
    }
    
    if(app.keyboard[SDL_SCANCODE_D])
    {
        self->dx     =  P_RUN_SPEED;
        self->facing =  FACING_RIGHT; 
    }

    if(app.keyboard[SDL_SCANCODE_W])
    {
        self->dy     = -P_RUN_SPEED;
        self->facing =  FACING_UP;
    }

    if(app.keyboard[SDL_SCANCODE_S])
    {
        self->dy     =  P_RUN_SPEED;
        self->facing =  FACING_DOWN;
    }
}

static void die(entity_t *self) {}

static void do_hitbox(entity_t *self)
{
    self->hitbox.x = self->x;
    self->hitbox.y = self->y;
    self->hitbox.w = self->texture->rect.w;
    self->hitbox.h = self->texture->rect.h;
}

static int is_control(int type)
{
    int key, btn;

    /* include player control defs below this line */
    // if(type == CTRL_LEFT && app.joypad_axis[JOYPAD_AXIS_X] < -game.deadzone)
    //     return 1;

    if(type == CTRL_LEFT && app.joypad_axis[JAXIS_X] < -game.deadzone)
        return 1;
    if(type == CTRL_RIGHT && app.joypad_axis[JAXIS_X] > game.deadzone)
        return 1;

    key = game.keyboard_controls[type];
    btn = game.joypad_controls[type];

    return ((key != 0 && app.keyboard[key]) || (btn != -1 && app.joypad_button[btn]));
}