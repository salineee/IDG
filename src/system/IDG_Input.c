#include "../IDG_Common.h"

#include "IDG_Input.h"

extern app_t app;

// KEYBOARD
static void IDG_DoKeyup(SDL_KeyboardEvent *ev)
{
    if(ev->repeat == 0 && ev->keysym.scancode < MAX_KEYBOARD_EVENTS)
        app.keyboard[ev->keysym.scancode] = 0;
}

static void IDG_DoKeydown(SDL_KeyboardEvent *ev)
{
    if(ev->repeat == 0 && ev->keysym.scancode < MAX_KEYBOARD_EVENTS)
    {
        app.keyboard[ev->keysym.scancode] = 1;
        app.last_key_pressed = ev->keysym.scancode;
    }
}

// MOUSE
static void IDG_DoMBup(SDL_MouseButtonEvent *ev)
{
    app.mouse.buttons[ev->button] = 0;
}

static void IDG_DoMBdown(SDL_MouseButtonEvent *ev)
{
    app.mouse.buttons[ev->button] = 1;
}

// MOUSEWHEEL
static void IDG_DoMW(SDL_MouseWheelEvent *ev)
{
    if(ev->y == -1)
        app.mouse.buttons[SDL_BUTTON_X1] = 1;
    if(ev->y == 1)
        app.mouse.buttons[SDL_BUTTON_X2] = 1;
}

// CONTROLLER BUTTONS
static void IDG_DoCBup(SDL_JoyButtonEvent *ev)
{
    if(ev->state == SDL_RELEASED && ev->button < MAX_JOYPAD_BUTTONS)
        app.joypad_button[ev->button] = 0;
}

static void IDG_DoCBdown(SDL_JoyButtonEvent *ev)
{
    if(ev->state == SDL_PRESSED && ev->button < MAX_JOYPAD_BUTTONS)
    {
        app.joypad_button[ev->button] = 1;
        app.last_button_pressed = ev->button;
    }
}

// CONTROLLER AXIS
static void IDG_DoJAxis(SDL_JoyAxisEvent *ev)
{
    if(ev->axis < JAXIS_MAX)
        app.joypad_axis[ev->axis] = ev->value;
}

// WINDOW FULLSCREEN
static void IDG_ToggleWindowFullscreen(void)
{
    // TODO - scale window appropriately
    Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
    bool IsFullscreen = SDL_GetWindowFlags(app.window) & FullscreenFlag;
    SDL_SetWindowFullscreen(app.window, IsFullscreen ? 0 : FullscreenFlag);
    SDL_ShowCursor(IsFullscreen);
}

void IDG_DoInput(void)
{
    SDL_Event ev;
    while(SDL_PollEvent(&ev))
    {
        switch(ev.type)
        {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_WINDOW_FULLSCREEN:
                IDG_ToggleWindowFullscreen();
                break;
            case SDL_KEYDOWN:
                IDG_DoKeydown (&ev.key);
                break;
            case SDL_KEYUP:
                IDG_DoKeyup   (&ev.key);
                break;
            case SDL_MOUSEBUTTONDOWN:
                IDG_DoMBdown  (&ev.button);
                break;
            case SDL_MOUSEBUTTONUP:
                IDG_DoMBup    (&ev.button);
                break;
            case SDL_MOUSEWHEEL:
                IDG_DoMW      (&ev.wheel);
                break;
            case SDL_JOYBUTTONDOWN:
                IDG_DoCBdown  (&ev.jbutton);
                break;
            case SDL_JOYBUTTONUP:
                IDG_DoCBup    (&ev.jbutton);
                break;
            case SDL_JOYAXISMOTION:
                IDG_DoJAxis   (&ev.jaxis);
                break;
            default:
                break;
        }
    }

    /**********************************************
     * Toggle fullscreen via keypress.
     * To toggle via window buttons, see IDG_Init.c
     * in windowflags.
    **********************************************/
    if(app.keyboard[SDL_SCANCODE_F12])
    {
        Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
        bool IsFullscreen = SDL_GetWindowFlags(app.window) & FullscreenFlag;
        SDL_SetWindowFullscreen(app.window, IsFullscreen ? 0 : FullscreenFlag);
        SDL_ShowCursor(IsFullscreen);
    }

    SDL_GetMouseState(&app.mouse.x, &app.mouse.y);
}