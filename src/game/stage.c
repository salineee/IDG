#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Camera.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Effect.h"
#include "../system/IDG_Entities.h"
#include "../system/IDG_EntityFactory.h"
#include "../system/IDG_Hud.h"
#include "../system/IDG_Map2D.h"
#include "../system/IDG_Options.h"
#include "../system/IDG_Quadtree.h"
#include "../system/IDG_Text.h"
#include "../system/IDG_Util.h"
#include "../system/IDG_Widgets.h"
#include "../editor/IDG_Sector2D.h"
#include "../json/cJSON.h"
#include "../entities/player.h"

#include "stage.h"

extern app_t   app;
extern stage_t stage;

static void logic         (void);
static void draw          (void);
static void load_stage    (char *filename);

static void do_pause      (void);
static void do_stage      (void);
static void reset_stage   (void);

static void draw_stage    (void);
static void setup_widgets (void);

static int was_init = 0;
static int paused;

void init_stage(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "*** LOADING STAGE '%s' IN %s MODE ***\n", stage.name, (app.dev.is_debug ? "DEBUG" : "NORMAL"));

    if(!was_init)
    {
        was_init = 1;

        /* load stage textures below this line */

        // setup_widgets();
    }

    IDG_InitQuadtree (&stage.quadtree);
    IDG_InitMap2D    ();
    IDG_InitSector2D ();
    IDG_LoadSector2D (); // TODO - this should be consolidated into InitSector2D()
    IDG_InitEntities ();
    IDG_InitHUD      ();
    IDG_InitEffects  ();

    char stage_filename[MAX_FILENAME_LENGTH];
    sprintf          (stage_filename, "data/stages/%s.json", stage.name);
    load_stage       (stage_filename);
    
    IDG_InitCamera   ();

    app.delegate.logic = logic;
    app.delegate.draw  = draw;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "*** FINISHED LOADING STAGE '%s' IN %s MODE ***\n\n", stage.name, (app.dev.is_debug ? "DEBUG" : "NORMAL"));
}

static void logic(void)
{
    if(!paused) { do_stage(); }
    else        { do_pause(); }
}

static void draw(void)
{
    draw_stage();

    if(!paused)
    {
        IDG_DrawHUD();
    }
    else
    {
        IDG_DrawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00, 0x00, 0x00, 160);
        app.font_scale = 1.5;
        IDG_DrawText("PAUSE", (SCREEN_WIDTH/2), (SCREEN_HEIGHT/2), 0xFF, 0xFF, 0xFF, TEXT_ALIGN_CENTER, 0); 
        app.font_scale = 1.0;
        // IDG_DrawWidgets("pause");
    }
}

static void do_stage(void)
{
    app.dev.collision_checks = 0; // reset collisions counter every frame for recalculation

    IDG_DoEntities ();
    // IDG_DoEffects  ();
    IDG_DoHUD      ();

    if(stage.camera.initialized) { IDG_DoCamera(); }

    /* special controls... pausing, and dev stuff */
    // TODO - i really wanna get this out of stage.c
    if(app.keyboard[SDL_SCANCODE_ESCAPE])
    {
        app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
        // app.active_widget = IDG_GetWidget("resume", "pause");
        paused = 1;
    }

    if(app.keyboard[SDL_SCANCODE_F1])
    {
        app.keyboard[SDL_SCANCODE_F1] = 0;
        app.dev.show_debug = !app.dev.show_debug;
    }

    if(app.keyboard[SDL_SCANCODE_F2])
    {
        app.keyboard[SDL_SCANCODE_F2] = 0;
        app.dev.show_hitboxes = !app.dev.show_hitboxes;
    }
    
    if(app.keyboard[SDL_SCANCODE_F3])
    {
        app.keyboard[SDL_SCANCODE_F3] = 0;
        app.dev.show_sectors = !app.dev.show_sectors;
    }
}

static void do_pause(void)
{
    // IDG_DoWidgets("pause");

    if(app.keyboard[SDL_SCANCODE_ESCAPE])
    {
        app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
        paused = 0;
    }
}

static void draw_stage(void)
{
    IDG_DrawEntities (LAYER_BACKGROUND);
    IDG_DrawMap2D    ();

    if(app.dev.show_debug && app.dev.show_sectors)
        IDG_DrawSector2D();

    IDG_DrawEntities (LAYER_FOREGROUND);
    IDG_DrawEffects  ();
}

static void load_stage(char *filename)
{
    cJSON    *root, *node;
    entity_t *e;
    char     *text, *type;
    int       x, y;

    text = IDG_ReadFile(filename);
    root = cJSON_Parse(text);

    for(node=root->child; node!=NULL; node=node->next)
    {
        type = cJSON_GetObjectItem(node, "type")->valuestring;
        x    = cJSON_GetObjectItem(node, "x")->valueint;
        y    = cJSON_GetObjectItem(node, "y")->valueint;

        e    = IDG_InitEntity(type);
        e->x = x;
        e->y = y;
    }

    cJSON_Delete(root);
    free(text);
}

static void reset_stage(void)
{
    IDG_DestroyQuadtree(&stage.quadtree);
    IDG_ClearEntities();
    IDG_ClearEffects();

    free(stage.player->data);
    free(stage.player);
}

static void resume(void)
{
    paused = 0;
}

static void post_options(void)
{
    app.active_widget  = IDG_GetWidget("options", "pause");
    app.delegate.logic = logic;
    app.delegate.draw  = draw;
}

static void options(void)
{
    IDG_InitOptions(NULL, draw_stage, post_options);
}

static void quit(void)
{
    exit(0);
}

static void setup_widgets(void)
{
    widget_t *w;

    // w = IDG_GetWidget("resume", "pause");
    w->x = ((SCREEN_WIDTH-w->w)/2);
    w->action = resume;

    // w = IDG_GetWidget("options", "pause");
    w->x = ((SCREEN_WIDTH-w->w)/2);
    w->action = options;

    // w = IDG_GetWidget("quit", "pause");
    w->x = ((SCREEN_WIDTH-w->w)/2);
    w->action = quit;
}