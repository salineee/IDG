#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"
#include "../system/IDG_Text.h"
#include "../system/IDG_Util.h"
#include "../json/cJSON.h"

#include "IDG_Sector2D.h"

extern app_t   app;
extern stage_t stage;

static sector_2D_t *IDG_AddSector2D (void);

static SDL_Point c1, c2; // sector corner 1, sector corner 2
static int 		 s2D_x,  // sector2D x  
				 s2D_y,  // sector2D y
                 s2D_x1, // sector2D x1
                 s2D_x2, // sector2D x2
                 s2D_y1, // sector2D y1
                 s2D_y2, // sector2D y2
                 s2D_w,  // sector2D width
                 s2D_h;  // sector2D height

void IDG_InitSector2D(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Initializing Sector2D ...");
    
    memset(&stage.sector2D_head, 0, sizeof(sector_2D_t));
    stage.sector2D_tail = &stage.sector2D_head;
}

static char *IDG_GetSector2DFilename(void)
{
    return IDG_FormattedString("data/stages/%s_sectors.json", stage.name);
}

void IDG_LoadSector2D(void)
{
    sector_2D_t  *s;
    cJSON        *root, *node;
    char         *filename, *text;
    int           sector_id, layer,
                  interactive, solid, blocks_los,
                  x, y, w, h;
    unsigned long flags;

    filename = IDG_GetSector2DFilename();
    text     = IDG_ReadFile(filename);
    root     = cJSON_Parse(text);

    for(node=root->child; node!=NULL; node=node->next)
    {
        sector_id   = cJSON_GetObjectItem ( node, "sector_id"   )->valueint;
        x           = cJSON_GetObjectItem ( node, "x"           )->valueint;
        y           = cJSON_GetObjectItem ( node, "y"           )->valueint;
        w           = cJSON_GetObjectItem ( node, "w"           )->valueint;
        h           = cJSON_GetObjectItem ( node, "h"           )->valueint;
        layer       = cJSON_GetObjectItem ( node, "layer"       )->valueint;
        interactive = cJSON_GetObjectItem ( node, "interactive" )->valueint;
        solid       = cJSON_GetObjectItem ( node, "solid"       )->valueint;
        blocks_los  = cJSON_GetObjectItem ( node, "blocks_los"  )->valueint;

        s = IDG_AddSector2D();
        s->sector_id   = sector_id;
        s->x           = x;
        s->y           = y;
        s->w           = w;
        s->h           = h;
        s->layer       = layer;
        s->interactive = interactive;
        s->solid       = solid;
        s->blocks_los  = blocks_los;
    }

    cJSON_Delete (root);
    free         (text);
}

void IDG_DrawSector2D(void)
{
    sector_2D_t *s;
    char text[MAX_NAME_LENGTH];

    // draw all committed sectors
    for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
    {
        if(s->solid)
        {
            IDG_DrawRect(
                (s->x-stage.camera.pos.x),
                (s->y-stage.camera.pos.y),
                s->w, s->h,
                0xFF, 0x00, 0x00, 0x64
            );
        }
        else
        {
            IDG_DrawRect(
                (s->x-stage.camera.pos.x),
                (s->y-stage.camera.pos.y),
                s->w, s->h,
                0x00, 0x00, 0xFF, 0x64
            );
        }

        sprintf(text, "%d", s->sector_id);
        IDG_DrawText(
            text, 
            ((s->x)-stage.camera.pos.x), ((s->y)-stage.camera.pos.y),
            255, 255, 255,
            TEXT_ALIGN_LEFT, 0
        );
    }
}

void IDG_Sector2DDrawSelectionPreview(SDL_Point mouse_tile)
{
    if(c1.x != 0 && c2.x == 0)
    {
        s2D_x = (MAX(c1.x, mouse_tile.x)*MAP_TILE_SIZE)-stage.camera.pos.x;
        s2D_y = (MAX(c1.y, mouse_tile.y)*MAP_TILE_SIZE)-stage.camera.pos.y;

        s2D_x1 = (c1.x*MAP_TILE_SIZE)-stage.camera.pos.x;
        s2D_x2 = (mouse_tile.x*MAP_TILE_SIZE)-stage.camera.pos.x;
        s2D_y1 = (c1.y*MAP_TILE_SIZE)-stage.camera.pos.y;
        s2D_y2 = (mouse_tile.y*MAP_TILE_SIZE)-stage.camera.pos.y;
    
        s2D_w  = ((s2D_x1-s2D_x2) > 0 ? ((s2D_x1-s2D_x2)*-1) : (s2D_x1-s2D_x2));
        s2D_h  = ((s2D_y1-s2D_y2) > 0 ? ((s2D_y1-s2D_y2)*-1) : (s2D_y1-s2D_y2));

        IDG_DrawRect(s2D_x, s2D_y, s2D_w, s2D_h, 255, 0, 0, 100);
    }
}

void IDG_Sector2DDrawHoverState(void)
{
    if(app.dev.show_sectors)
    {
        sector_2D_t *s;
        for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
            if(IDG_Collision(app.mouse.x, app.mouse.y, 1, 1, (s->x-stage.camera.pos.x), (s->y-stage.camera.pos.y), s->w, s->h))
                IDG_DrawRect((s->x-stage.camera.pos.x), (s->y-stage.camera.pos.y), s->w, s->h, 255, 0, 0, 100);
    }
}

static sector_2D_t *IDG_AddSector2D(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Creating Sector2D ...");

    sector_2D_t *s;
    s = malloc(sizeof(sector_2D_t));
    memset(s, 0, sizeof(sector_2D_t));
    stage.sector2D_tail->next = s;
    stage.sector2D_tail       = s;
    return s;
}

void IDG_Sector2DSetPoint(SDL_Point mouse_tile)
{
    // TODO - this should wrongfully prevent a sector from beginning at 0,0
    if(c1.x == 0 && c2.x == 0) { c1 = mouse_tile; }
    else if(c1.x != 0 && c2.x == 0)
    { 
        c2 = mouse_tile;
        
        sector_2D_t *s;

        int s2D_cnt;
        for(s=stage.sector2D_head.next, s2D_cnt=0; s!=NULL; s=s->next, s2D_cnt++);

        s = IDG_AddSector2D();

        s2D_x = (MIN(c1.x, c2.x)*MAP_TILE_SIZE); // rect origin - top left
        s2D_y = (MIN(c1.y, c2.y)*MAP_TILE_SIZE); // rect origin - top left

        s2D_x1 = (c1.x*MAP_TILE_SIZE);
        s2D_x2 = (c2.x*MAP_TILE_SIZE);
        s2D_y1 = (c1.y*MAP_TILE_SIZE);
        s2D_y2 = (c2.y*MAP_TILE_SIZE);
    
        s2D_w  = ((s2D_x1-s2D_x2) < 0 ? ((s2D_x1-s2D_x2)*-1) : (s2D_x1-s2D_x2));
        s2D_h  = ((s2D_y1-s2D_y2) < 0 ? ((s2D_y1-s2D_y2)*-1) : (s2D_y1-s2D_y2));

        s->sector_id = (s2D_cnt+1);
        s->x         = s2D_x;
        s->y         = s2D_y;
        s->w         = s2D_w;
        s->h         = s2D_h;

        IDG_Sector2DResetSelection(); // reset corners
    }
}

void IDG_DeleteSector2D(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Clearing Sector %d ... Save to confirm.", app.dev.curr_sector_id);

	sector_2D_t *s, *prev;
	prev = &stage.sector2D_head;
	for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
	{
		if(s->sector_id == app.dev.curr_sector_id)
		{
			prev->next = s->next;
			if(s == stage.sector2D_tail)
				stage.sector2D_tail = prev;
			free(s);
			s = prev;
		}
		prev = s;
	}
    app.dev.sector_menu_focused = 0;
}

int IDG_SaveSector2D(void)
{
    sector_2D_t *s;
    cJSON       *root, *sector2D_JSON;
    char        *filename, *out;
    int          rtn;

    root = cJSON_CreateArray();

    for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
    {
        sector2D_JSON = cJSON_CreateObject();
        cJSON_AddNumberToObject ( sector2D_JSON, "sector_id",   s->sector_id   );
        cJSON_AddNumberToObject ( sector2D_JSON, "x",           s->x           );
        cJSON_AddNumberToObject ( sector2D_JSON, "y",           s->y           );
        cJSON_AddNumberToObject ( sector2D_JSON, "w",           s->w           );
        cJSON_AddNumberToObject ( sector2D_JSON, "h",           s->h           );
        cJSON_AddNumberToObject ( sector2D_JSON, "layer",       LAYER_SECTOR   );

        // flags
        cJSON_AddNumberToObject ( sector2D_JSON, "solid",       s->solid       );
        cJSON_AddNumberToObject ( sector2D_JSON, "interactive", s->interactive );
        cJSON_AddNumberToObject ( sector2D_JSON, "blocks_los",  s->blocks_los  );
        
        // TODO - get rid of these NULLs
        cJSON_AddNullToObject   ( sector2D_JSON, "material" );

        cJSON_AddItemToArray(root, sector2D_JSON);
    }

    out      = cJSON_Print             (root);
    filename = IDG_GetSector2DFilename ();
    rtn      = IDG_WriteFile           (filename, out);

    cJSON_Delete (root);
    free         (filename);
    free         (out);

    return rtn;
}

// initialize sector corner selections to empty
void IDG_Sector2DResetSelection(void)
{
    c1.x = c1.y = c2.x = c2.y = 0;
}

// TODO - is there a way to combine/reuse some of this code?
void IDG_Sector2DSetBlocksLOS(void)
{
	select_widget_t *sw;
	sector_2D_t     *s;
	sw = (select_widget_t *)app.active_widget->data;
	for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
		if(s->sector_id == app.dev.curr_sector_id)
			s->blocks_los = sw->value;
}

void IDG_Sector2DSetSolid(void)
{
	select_widget_t *sw;
	sector_2D_t     *s;
	sw = (select_widget_t *)app.active_widget->data;
	for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
		if(s->sector_id == app.dev.curr_sector_id)
			s->solid = sw->value;
}

void IDG_Sector2DSetInteractive(void)
{
	select_widget_t *sw;
	sector_2D_t     *s;
	sw = (select_widget_t *)app.active_widget->data;
	for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
		if(s->sector_id == app.dev.curr_sector_id)
			s->interactive = sw->value;
}