#include <SDL2/SDL_mixer.h>

#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"
#include "../system/IDG_Sound.h"
#include "../system/IDG_Text.h"
#include "../system/IDG_Widgets.h"
#include "../system/IDG_Game.h"

#include "IDG_Options.h"

extern app_t  app;
extern game_t game;

static void IDG_SetupWidgets (void);
static void logic            (void);
static void draw             (void);
static void back             (void);
static void back_to_options  (void);

static void (*pre_logic)    (void);
static void (*pre_draw)     (void);
static void (*post_options) (void);
static int was_init = 0;
static int show_controls;

void IDG_InitOptions(void (*_pre_logic)(void), void (*_pre_draw)(void), void (*_post_options)(void))
{
    pre_logic    = _pre_logic;
    pre_draw     = _pre_draw;
    post_options = _post_options;

    if(!was_init)
    {
        IDG_SetupWidgets();
        was_init = 1;
    }

    show_controls      = 0;
    // app.active_widget  = IDG_GetWidget("sound", "options");
    app.delegate.logic = logic;
    app.delegate.draw  = draw;
}

static void logic(void)
{
    if(pre_logic != NULL)
        pre_logic();
    
    if(!show_controls)
    {
        // IDG_DoWidgets("options");

        if(app.keyboard[SDL_SCANCODE_ESCAPE])
        {
            app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
            back();
        }
    }
    else
    {
        // IDG_DoWidgets("controls");

        if(app.keyboard[SDL_SCANCODE_ESCAPE])
        {
            app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
            back_to_options();
        }
    }
}

static void draw(void)
{
    pre_draw();
    IDG_DrawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 160);

    if(!show_controls)
    {
        app.font_scale = 1.5;
        IDG_DrawText("OPTIONS", (SCREEN_WIDTH/2), 70, 255, 255, 255, TEXT_ALIGN_CENTER, 0);
        app.font_scale = 1.0;
        // IDG_DrawWidgets("options");
    }
    else
    {
        app.font_scale = 1.5;
        IDG_DrawText("CONTROLS", (SCREEN_WIDTH/2), 70, 255, 255, 255, TEXT_ALIGN_CENTER, 0);
        app.font_scale = 1.0;
        // IDG_DrawWidgets("controls");
    }
}

static void sound(void)
{
    slider_widget_t *sw;
    sw = (slider_widget_t *)app.active_widget->data;

    game.sound_volume = MIX_MAX_VOLUME*(sw->value*0.01);
    IDG_SetSoundVolume(game.sound_volume);
}

static void music(void)
{
    slider_widget_t *sw;
    sw = (slider_widget_t *)app.active_widget->data;

    game.music_volume = MIX_MAX_VOLUME*(sw->value*0.01);
    IDG_SetMusicVolume(game.music_volume);
}

static void controls(void)
{
    show_controls = 1;
    // app.active_widget = IDG_GetWidget("left", "controls");
}

static void deadzone(void)
{
    slider_widget_t *sw;
    sw = (slider_widget_t *)app.active_widget->data;

    game.deadzone = DEADZONE_MAX*(sw->value*0.01);
}

static void back(void)
{
    IDG_SaveGame();
    post_options();
}

static void back_to_options(void)
{
    show_controls = 0;
    // app.active_widget = IDG_GetWidget("controls", "options");
}

static void update_controls(void)
{
    control_widget_t *w;

    // w = (control_widget_t *)IDG_GetWidget("left",  "controls")->data;
    // game.keyboard_controls[CTRL_LEFT] = w->keyboard;
    // game.joypad_controls[CTRL_LEFT]   = w->joypad;

    // w = (control_widget_t *)IDG_GetWidget("right", "controls")->data;
    // game.keyboard_controls[CTRL_RIGHT] = w->keyboard;
    // game.joypad_controls[CTRL_RIGHT]   = w->joypad;
    
    // w = (control_widget_t *)IDG_GetWidget("up",    "controls")->data;
    // game.keyboard_controls[CTRL_UP] = w->keyboard;
    // game.joypad_controls[CTRL_UP]   = w->joypad;

    // w = (control_widget_t *)IDG_GetWidget("down",  "controls")->data;
    // game.keyboard_controls[CTRL_DOWN] = w->keyboard;
    // game.joypad_controls[CTRL_DOWN]   = w->joypad;
    
    // w = (control_widget_t *)IDG_GetWidget("jump",  "controls")->data;
    // game.keyboard_controls[CTRL_JUMP] = w->keyboard;
    // game.joypad_controls[CTRL_JUMP]   = w->joypad;

    // w = (control_widget_t *)IDG_GetWidget("fire",  "controls")->data;
    // game.keyboard_controls[CTRL_FIRE] = w->keyboard;
    // game.joypad_controls[CTRL_FIRE]   = w->joypad;
}

static void IDG_SetupWidgets(void)
{
    widget_t         *w;
    slider_widget_t  *sw;
    control_widget_t *cw;

    // w = IDG_GetWidget("sound", "options");
    // w->action = sound;
    // sw = (slider_widget_t *)w->data;
    // sw->x = (w->x+175);
    // sw->y = (w->y+16);
    // sw->w = 350;
    // sw->h = 32;
    // sw->value = (((1.0*game.sound_volume)/MIX_MAX_VOLUME)*100);

    // w = IDG_GetWidget("music", "options");
    // w->action = music;
    // sw = (slider_widget_t *)w->data;
    // sw->x = (w->x+175);
    // sw->y = (w->y+16);
    // sw->w = 350;
    // sw->h = 32;
    // sw->value = (((1.0*game.music_volume)/MIX_MAX_VOLUME)*100);

	// w = IDG_GetWidget("controls", "options");
	// w->action = controls;

	// w = IDG_GetWidget("back", "options");
	// w->action = back;

	// w = IDG_GetWidget("left", "controls");
	// w->action = update_controls;
	// cw = (control_widget_t *)w->data;
	// cw->x        = (w->x+200);
	// cw->y        = w->y;
	// cw->keyboard = game.keyboard_controls[CTRL_LEFT];
	// cw->joypad   = game.joypad_controls[CTRL_LEFT];

	// w = IDG_GetWidget("right", "controls");
	// w->action = update_controls;
	// cw = (control_widget_t *)w->data;
	// cw->x        = (w->x+200);
	// cw->y        = w->y;
	// cw->keyboard = game.keyboard_controls[CTRL_RIGHT];
	// cw->joypad   = game.joypad_controls[CTRL_RIGHT];

	// w = IDG_GetWidget("up", "controls");
	// w->action = update_controls;
	// cw = (control_widget_t *)w->data;
	// cw->x        = (w->x+200);
	// cw->y        = w->y;
	// cw->keyboard = game.keyboard_controls[CTRL_UP];
	// cw->joypad   = game.joypad_controls[CTRL_UP];

	// w = IDG_GetWidget("down", "controls");
	// w->action = update_controls;
	// cw = (control_widget_t *)w->data;
	// cw->x        = (w->x+200);
	// cw->y        = w->y;
	// cw->keyboard = game.keyboard_controls[CTRL_DOWN];
	// cw->joypad   = game.joypad_controls[CTRL_DOWN];

	// w = IDG_GetWidget("jump", "controls");
	// w->action = update_controls;
	// cw = (control_widget_t *)w->data;
	// cw->x        = (w->x+200);
	// cw->y        = w->y;
	// cw->keyboard = game.keyboard_controls[CTRL_JUMP];
	// cw->joypad   = game.joypad_controls[CTRL_JUMP];

	// w = IDG_GetWidget("fire", "controls");
	// w->action = update_controls;
	// cw = (control_widget_t *)w->data;
	// cw->x        = (w->x+200);
	// cw->y        = w->y;
	// cw->keyboard = game.keyboard_controls[CTRL_FIRE];
	// cw->joypad   = game.joypad_controls[CTRL_FIRE];

    // w = IDG_GetWidget("deadzone", "controls");
    // w->action = deadzone;
    // sw = (slider_widget_t *)w->data;
    // sw->x = (w->x+225);
    // sw->y = (w->y+16);
    // sw->w = 350;
    // sw->h = 32;
    // sw->value = (((1.0*game.deadzone)/DEADZONE_MAX)*100);

    // w = IDG_GetWidget("back", "controls");
    // w->action = back_to_options;

    // update_controls();
}
