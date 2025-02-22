#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Text.h"
#include "../system/IDG_Util.h"

#include "IDG_Hud.h"

extern app_t   app;
extern stage_t stage;

static void IDG_DrawDebugHUD(void);

void IDG_InitHUD(void) {}

void IDG_DoHUD(void) {}

void IDG_DrawHUD(void)
{
    /* draw player's hud below this line */
    if(app.dev.show_debug) { IDG_DrawDebugHUD(); }
}

static void IDG_DrawDebugHUD(void)
{
    char text[64];
    app.font_scale = 0.5;

    if(stage.player != NULL)
    {
        sprintf(text, "x: %d, y: %d", (int)stage.player->x, (int)stage.player->y);
        IDG_DrawText(text, 10, 10, 0x00, 0x00, 0x00, TEXT_ALIGN_LEFT, 0);

        sprintf(text, "friction: %.3lf, dx: %.3lf, dy: %.3lf", stage.player->friction, stage.player->dx, stage.player->dy);
        IDG_DrawText(text, 10, 30, 0x00, 0x00, 0x00, TEXT_ALIGN_LEFT, 0);
    }

    sprintf(text, "FPS: %d", app.dev.fps);
    IDG_DrawText(text, 10, 50, 0x00, 0x00, 0x00, TEXT_ALIGN_LEFT, 0);

    sprintf(text, "Collisions: %d", app.dev.collision_checks);
    IDG_DrawText(text, 10, 70, 0x00, 0x00, 0x00, TEXT_ALIGN_LEFT, 0);

    sprintf(text, "Display Hitboxes: %s", (app.dev.show_hitboxes ? "TRUE" : "FALSE"));
    IDG_DrawText(text, 10, 90, 0x00, 0x00, 0x00, TEXT_ALIGN_LEFT, 0);

    sprintf(text, "Display Sector2D: %s", (app.dev.show_sectors ? "TRUE" : "FALSE"));
    IDG_DrawText(text, 10, 110, 0x00, 0x00, 0x00, TEXT_ALIGN_LEFT, 0);

    app.font_scale = 1.0;
}