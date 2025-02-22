#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <time.h>

#include "../IDG_Common.h"

#include "IDG_Atlas.h"
#include "IDG_EntityFactory.h"
#include "IDG_Game.h"
#include "IDG_Sound.h"
#include "IDG_Text.h"
#include "IDG_Textures.h"
#include "IDG_Widgets.h"

#include "IDG_Init.h"

void IDG_InitSDL(void)
{
    int renderer_flags, window_flags;
    renderer_flags = SDL_RENDERER_ACCELERATED;
    window_flags   = 0;
    // window_flags   = SDL_WINDOW_RESIZABLE; // this seems buggy

    /* INIT SDL */
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0)
    {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    else printf("Initialized SDL\n");

    /* INIT MIXER */

    /*** NOTES ********************************************
     * Mix_OpenAudio args can be important for performance:
     * an older machine may want to use a freq of 22050
     * https://www.libsdl.org...ojects/SDL_mixer/docs/SDL
     *****************************************************/
    if(Mix_OpenAudio(
        SND_FREQ,           // frequency - 44100 is CD quality 
        MIX_DEFAULT_FORMAT, // format
        2,                  // channels
        SND_CHUNK_SIZE      // chunk size
    ) == -1)
    {
        printf("Couldn't initialize SDL Mixer\n");
        exit(1);
    }
    else printf("Initialized SDL Mixer\n");

    Mix_AllocateChannels(SND_MAX_CHANNELS);

    /* INIT WINDOW */
    app.window = SDL_CreateWindow(
        "IDG_ENG_1.0",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        window_flags
    );

    if(!app.window)
    {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }
    else printf("Opened SDL Window\n");

    /* INIT RENDERER */
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    app.renderer = SDL_CreateRenderer(app.window, -1, renderer_flags);

    if(!app.renderer)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
    else printf("Created SDL Renderer\n");

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    if (TTF_Init() < 0)
	{
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
		exit(1);
	}

    SDL_ShowCursor(0);
}

static void IDG_InitJoypad(void)
{
    int i, n;
    n = SDL_NumJoysticks();

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%d joysticks available", n);

    for(i=0; i<n; i++)
    {
        app.joypad = SDL_JoystickOpen(i);
        if(app.joypad != NULL)
        {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Joystick [name='%s', Axes=%d, Buttons=%d]", SDL_JoystickNameForIndex(i), SDL_JoystickNumAxes(app.joypad), SDL_JoystickNumButtons(app.joypad));
            return;
        }
    }
}

void IDG_InitGameSystem(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "*** INITIALIZING ENGINE ***\n");
    
    srand(time(NULL));

    IDG_InitJoypad        ();
    IDG_InitFonts         ();
    IDG_InitAtlas         ();
    IDG_InitSound         ();
    IDG_InitTextures      ();
    IDG_InitEntityFactory ();
    IDG_InitWidgets       ();
    IDG_InitGame          ();

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "*** FINISHED INITIALIZING ENGINE ***\n\n");
}

void IDG_Cleanup(void)
{
    Mix_Quit            ();
    TTF_Quit            ();
    IMG_Quit            ();
    SDL_DestroyRenderer (app.renderer);
    SDL_DestroyWindow   (app.window);
    SDL_Quit            ();
}