#include "IDG_Common.h"

#include "editor/IDG_Editor2D.h"
#include "system/IDG_Draw.h"
#include "system/IDG_Init.h"
#include "system/IDG_Input.h"
#include "game/stage.h"

#include "main.h"

app_t   app;
game_t  game;
stage_t stage;

static void logic           (void);
static void do_fps          (void);                   // Handle framerate, deltatime
static void handle_cmd_line (int argc, char *argv[]); // Handle launch options for editor, atlas (TODO), etc.

static int  fps;
static long next_fps;

int main(int argc, char *argv[])
{
    long then;
    
    memset(&app, 0, sizeof(app_t));

	app.texture_scale = 1;

    IDG_InitSDL        ();
    IDG_InitGameSystem ();
    handle_cmd_line    (argc, argv);

    atexit             (IDG_Cleanup);

    next_fps = SDL_GetTicks() + 1000;


    while(1)
    {
        then = SDL_GetTicks ();

        IDG_PrepareScene    ();
        IDG_DoInput         ();

        logic               ();
        app.delegate.draw   ();

        IDG_PresentScene    ();

        /* allow CPU/GPU to catch up */
        SDL_Delay           (1);

        app.delta_time = LOGIC_RATE*(SDL_GetTicks()-then);

        do_fps              ();
    }
    return 0;
}

static void logic(void)
{
    double tmp_delta;
    /* don't exceed target logic rate */
	while (app.delta_time > 1)
	{
		tmp_delta = app.delta_time;
		app.delta_time = 1;
		app.delegate.logic();
		app.delta_time = (tmp_delta - 1);
	}
	app.delegate.logic();
}

static void do_fps(void)
{
	fps++;
	if (SDL_GetTicks() >= next_fps)
	{
		app.dev.fps = fps;
		fps = 0;
		next_fps = SDL_GetTicks() + 1000;
	}
}

static void handle_cmd_line (int argc, char *argv[])
{
	// TODO - Don't require specific ordering for flags
	
	int ok = 0;
	if (argc >= 3)
	{
		memset(&stage, 0, sizeof(stage_t));
		STRCPY(stage.name, argv[2]);

		if (strcmp(argv[1], "--edit") == 0 || strcmp(argv[1], "-E") == 0)
		{ 
			app.dev.is_editor = 1;
			IDG_InitEditor2D();
			ok = 1;
		}
		else if (strcmp(argv[1], "--map") == 0 || strcmp(argv[1], "-M") == 0)
		{
			if(argv[3] != NULL && (strcmp(argv[3], "--debug") == 0 || strcmp(argv[3], "-D") == 0))
			{
				app.dev.is_debug      = 1;
				app.dev.show_debug    = 1;
				app.dev.show_sectors  = 1;
				app.dev.show_hitboxes = 1;
			}

			init_stage();
			ok = 1;
		}
	}

	if (!ok)
	{
		printf("ERROR: Use --edit <name> or --map <name>.\n       Use --debug to launch a map with debug options.\a");
		exit(1);
	}
}