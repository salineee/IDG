#include "../IDG_Common.h"

#include "../system/IDG_Map2D.h"
#include "../system/IDG_Quadtree.h"
#include "../system/IDG_Util.h"

#include "IDG_ai.h"

extern stage_t stage;

static int trace_map      (entity_t *e);
static int trace_entities (entity_t *e);

int can_see_player(entity_t *e)
{
    return trace_map(e) && trace_entities(e);
}

static int trace_map(entity_t *e)
{
    // RESEARCH - Bresenham's line algorithm
    // LOS check to the middle of the player's hitbox

    int x, y,
        sx, sy,
        ex, ey,
        x1, y1,
        x2, y2,
        mx1, my1,
        mx2, my2;

    sx = (e->x + (e->texture->rect.w/2));
    sy = e->y;

    ex = (stage.player->x + (stage.player->texture->rect.w/2));
    ey = stage.player->y;

    x1 = MIN(sx, ex);
    y1 = MIN(sy, ey);
    x2 = MAX(sx, ex);
    y2 = MAX(sy, ey);

    mx1 = (x1/MAP_TILE_SIZE);
    my1 = (y1/MAP_TILE_SIZE);
    mx2 = (x2/MAP_TILE_SIZE);
    my2 = (y2/MAP_TILE_SIZE);

    for(x=mx1; x<=mx2; x++)
    {
        for(y=my1; y<=my2; y++)
        {
            if(IDG_IsInsideMap2D(x, y) && stage.map[x][y] != 0 && IDG_LineRectCollide(x1, y1, x2, y2, (x*MAP_TILE_SIZE), (y*MAP_TILE_SIZE), MAP_TILE_SIZE, MAP_TILE_SIZE))
            {
                return 0;
            }
        }
    }
    return 1;
}

static int trace_entities(entity_t *e)
{
    entity_t *candidates[MAX_QT_CANDIDATES], *other;
    int i,
        sx, sy, // start x, start y
        ex, ey, // end x, end y
        x1, y1, // corners of rectangle
        x2, y2; // corners of rectangle
    
    // start x of the line, from entity's position + half its texture width
    // start y 
    sx = (e->x + (e->texture->rect.w/2));
    sy = e->y; 

    // end x of the line, from entity's position + half its texture width
    // end y
    ex = (stage.player->x + (stage.player->texture->rect.w/2));
    ey = stage.player->y;

    // create rectangular dimensions to pass to quadtree
    x1 = MIN(sx, ex);
    y1 = MIN(sy, ey);
    x2 = MAX(sx, ex);
    y2 = MAX(sy, ey);

    // get all ents within rect
    IDG_GetAllEntsWithin(x1, y1, x2, y2, candidates, e);

    for(i=0, other=candidates[0]; i<MAX_QT_CANDIDATES && other!=NULL; other=candidates[++i])
    {
        if((other->flags & (EF_SOLID|EF_BLOCKS_LOS)) && IDG_LineRectCollide(x1, y1, x2, y2, other->x, other->y, other->texture->rect.w, other->texture->rect.h))
            return 0; // line of sight blocked
    }
    return 1; // line of sight open
}