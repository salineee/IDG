#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"

#include "IDG_Brush.h"

#define MAX_BRUSH_SIZE 9

extern stage_t stage;

void IDG_SetBrushSize(int *brush_size, int dir)
{
    if(dir == 1)
    {
        *brush_size = *brush_size + 2; // clamp to odd number
        *brush_size = MIN((*brush_size), MAX_BRUSH_SIZE);
    }
    else
    {
        *brush_size = *brush_size - 2; // clamp to odd number
        *brush_size = MAX((*brush_size), 1);
    }
}

void IDG_DrawBrush(int brush_size, SDL_Point mouse_tile)
{
    if(brush_size <= 1)
        IDG_DrawOutlineRect(
            (mouse_tile.x*MAP_TILE_SIZE)-stage.camera.pos.x,                  // x
            (mouse_tile.y*MAP_TILE_SIZE)-stage.camera.pos.y,                  // y
            MAP_TILE_SIZE, MAP_TILE_SIZE,                                 // w, h
            255, 255, 0, 255                                              // r,g,b,a
        );
    else
        IDG_DrawOutlineRect(
            ((mouse_tile.x-(brush_size/2))*MAP_TILE_SIZE)-stage.camera.pos.x, // x
            ((mouse_tile.y-(brush_size/2))*MAP_TILE_SIZE)-stage.camera.pos.y, // y
            (MAP_TILE_SIZE*brush_size), (MAP_TILE_SIZE*brush_size),       // w, y
            255, 255, 0, 255                                              // r,g,b,a
        );
}
