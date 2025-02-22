#include "../IDG_Common.h"

// #include "../game/background.h"
#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Entities.h"
#include "../system/IDG_EntityFactory.h"
#include "../system/IDG_Text.h"
#include "../system/IDG_Textures.h"
#include "../system/IDG_Util.h"
#include "../system/IDG_Widgets.h"
#include "../json/cJSON.h"
#include "../entities/player.h"

#include "IDG_Brush.h"
#include "IDG_Editor2D.h"
#include "IDG_Editor_Map2D.h"
#include "IDG_Sector2D.h"
#include "IDG_UI2D.h"

extern app_t   app;
extern stage_t stage;

static void	     IDG_LoadTiles			   	   (void);
static void	     IDG_AddDefaultEntities    	   (void);
static void	     logic				       	   (void);
static void	     draw 		 		       	   (void);
static void      IDG_DoEditor                  (void);
static void      IDG_DoPause                   (void);
static void      IDG_DrawEditor                (void);
static void	     IDG_DoMouse               	   (void);
static void	     IDG_DoKeyboard            	   (void);
static void	     IDG_CycleTile             	   (int *i, int dir);
static void	     IDG_CycleEntity           	   (int *i, int dir);
static void	     IDG_AddEntity             	   (void);
static void	     IDG_RemoveEntity          	   (void);
static void      IDG_PickEntity            	   (void);
static void	     IDG_DrawEditor2DUI        	   (void);
static void	     IDG_DrawEditor2DTopBar    	   (void);
static void	     IDG_DrawEditor2DBottomBar 	   (void);
static char     *IDG_GetTileTypeName       	   (void);
static entity_t *IDG_FindExisting          	   (char *type_name);
static void      IDG_DrawEditor2DMinimap   	   (void);
static void      IDG_DrawStatusMessage     	   (void);
static void      IDG_SetStatusMessage      	   (char *fmt, ...);
static void      IDG_SetupWidgets              (void);

static atlas_image_t *tile_mode_texture;
static atlas_image_t *entity_mode_texture;
static atlas_image_t *sector_mode_texture;
static atlas_image_t *pick_mode_texture;
static atlas_image_t *grid_toggle_texture;
static atlas_image_t *save_texture;

static SDL_Point   	  mouse_tile;
static SDL_Rect       minimap_rect;
static char           status_message[MAX_DESCRIPTION_LENGTH];
static double         status_message_timer;
static int         	  current_tile;
static int            brush_size;
static atlas_image_t *tiles[MAX_TILES];
static entity_t      *current_entity;
static entity_t     **entities;
static double         move_timer;
static int            current_entity_index;
static int            total_entities;
static int            mode;
static char          *mode_text[] = {"Tiles", "Sector", "Entities", "Pick", "Brush"};

void IDG_InitEditor2D(void)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading Editor2D");
	SDL_ShowCursor 		   (1);

	if(tile_mode_texture == NULL)
	{
		tile_mode_texture   = IDG_GetAtlasImage ( "data/gfx/system/tile.png",     1 );
		entity_mode_texture = IDG_GetAtlasImage ( "data/gfx/system/entities.png", 1 );
		sector_mode_texture = IDG_GetAtlasImage ( "data/gfx/system/sectors.png",  1 );
		pick_mode_texture   = IDG_GetAtlasImage ( "data/gfx/system/pick.png",     1 );
		grid_toggle_texture = IDG_GetAtlasImage ( "data/gfx/system/grid.png",     1 );
		save_texture        = IDG_GetAtlasImage ( "data/gfx/system/save.png",     1 );
	}

	IDG_InitSector2D       ();
	IDG_LoadSector2D       ();
	IDG_LoadTiles  		   ();
	IDG_AddDefaultEntities ();
	IDG_Editor_InitMap2D   ();
	// add_default_entities ();

	app.dev.sector_menu_focused = 0;
	app.dev.show_minimap        = 0;
	app.dev.show_grid           = 1;
	app.dev.show_sectors        = 1;

	move_timer   				= 0;
	current_tile 				= 1;
	mode         				= MODE_TILES;
	// entities                    = get_editor_entities(&total_entities);
	// current_entity              = entities[0];

	IDG_Sector2DResetSelection();


	minimap_rect.w = (MAP_WIDTH*MINIMAP_CEL_SIZE);
	minimap_rect.h = (MAP_HEIGHT*MINIMAP_CEL_SIZE);
	minimap_rect.x = (SCREEN_WIDTH-(minimap_rect.w+10));
	minimap_rect.y = (SCREEN_HEIGHT-(minimap_rect.h+64));

	brush_size     = 1;

	IDG_SetupWidgets();
	app.active_widget           = IDG_GetWidget("delete", "sectors");
	app.delegate.logic 			= logic;
	app.delegate.draw  			= draw;
}

// TODO - Should work on omitting this function,
// in an effort to make code reusable.
// Just reinforce errors for missing required entities.
static void IDG_AddDefaultEntities(void)
{
	entity_t *e;

	// *** TODO ***
	// For whatever reason this crashes everything.
	// Figure that out...

	// if (stage.entity_head.next == NULL)
	// {
	// 	e 	 = IDG_InitEntity("player");
	// 	e->x = 50;
	// 	e->y = 300;
	// }
}

// Rework the camera...
// FIGURE OUT HOW TO MULTITASK THE CAMERA- 
// SHOULD NOT NEED TO BE TIED TO A STAGE!!!
static void logic(void)
{
	if(!app.dev.sector_menu_focused)
		IDG_DoEditor();
	else
		IDG_DoPause();
}

static void IDG_DoEditor(void)
{
	mouse_tile.x = (app.mouse.x+stage.camera.pos.x)/MAP_TILE_SIZE;
	mouse_tile.y = (app.mouse.y+stage.camera.pos.y)/MAP_TILE_SIZE;

	// if(current_entity != NULL)
	// {
	// 	current_entity->x  = (int)((app.mouse.x+stage.camera.pos.x)/ENTITY_SPACING);
	// 	current_entity->x *= ENTITY_SPACING;

	// 	current_entity->y  = (int)((app.mouse.y+stage.camera.pos.y)/ENTITY_SPACING);
	// 	current_entity->y *= ENTITY_SPACING;
	// }

	move_timer 			 = MAX((move_timer-app.delta_time), 0);
	status_message_timer = MAX((status_message_timer-app.delta_time), 0);

	IDG_DoMouse    ();
	IDG_DoKeyboard ();
}

static void IDG_DoPause(void)
{
	IDG_DoWidgets("sectors");
	
	if(app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		app.dev.sector_menu_focused = 0;
	}
}

static void draw(void)
{
	if(!app.dev.sector_menu_focused)
	{
		/* draw background, if applicable */

		if(app.dev.show_grid && mode == MODE_ENTITIES)
			IDG_DrawGridLines    (ENTITY_SPACING);

		IDG_Editor_DrawMap2D     (LAYER_BACKGROUND); // draw map foreground
		IDG_Editor_DrawMap2D     (LAYER_FOREGROUND); // draw map background

		if(app.dev.show_sectors)
			IDG_DrawSector2D     ();                 // draw sectors

		if(app.dev.show_grid && mode == MODE_ENTITIES)
			IDG_DrawGridLines    (ENTITY_SPACING);

		if(mode == MODE_SECTOR || app.dev.show_sectors)
		{
			IDG_Sector2DDrawSelectionPreview (mouse_tile);
			IDG_Sector2DDrawHoverState       ();
		}

		if(app.dev.show_grid)
			if(mode == MODE_TILES || mode == MODE_SECTOR)
				IDG_DrawGridLines (MAP_TILE_SIZE);

		if(mode == MODE_TILES)
		{
			/* draw tile at mouse pos */
			IDG_BlitAtlasImage    (tiles[current_tile], (mouse_tile.x*MAP_TILE_SIZE)-stage.camera.pos.x, (mouse_tile.y*MAP_TILE_SIZE)-stage.camera.pos.y, 0, SDL_FLIP_NONE);
			IDG_DrawBrush         (brush_size, mouse_tile);
		}
		// else if(current_entity != NULL)
		// {
		// 	current_entity->draw(current_entity);
		// 	draw_outline_rect ((current_entity->x-stage.camera.pos.x), (current_entity->y-stage.camera.pos.y), current_entity->w, current_entity->h, 255, 255, 0, 255);
		// }

		IDG_DrawEditor2DUI      ();
	}
	else
	{
		char text[64]; // TODO - arbitrary number... fix this

		IDG_DrawRect(0, 0, 450, SCREEN_HEIGHT, 0, 0, 0, 200);
		sprintf(text, "SET SECTOR %d METADATA", app.dev.curr_sector_id);
		IDG_DrawText(text, 16, 16, 255, 255, 255, TEXT_ALIGN_LEFT, 0);
		app.font_scale = 0.5;
		IDG_DrawWidgets("sectors");
		app.font_scale = 1.0;
	}
}

static void IDG_DoMouse(void)
{
	// TODO - can I remove a layer of nesting by
	// returning early if not inside map?
	if (IDG_Editor_IsInsideMap2D(mouse_tile.x, mouse_tile.y))
	{
		switch(mode)
		{
			case MODE_TILES:
				// PLACE TILE
				// *** TODO ***
				// Clean up this math.
				// Maybe the brush size calcs can be included into the mouse_tile pos?
				if(app.mouse.buttons[SDL_BUTTON_LEFT])
					for(int x=mouse_tile.x-(brush_size/2); x<=mouse_tile.x+(brush_size/2); x++)
						for(int y=mouse_tile.y-(brush_size/2); y<=mouse_tile.y+(brush_size/2); y++)
							stage.map[x][y] = current_tile;

				// REMOVE TILE
				if(app.mouse.buttons[SDL_BUTTON_RIGHT])
					for(int x=mouse_tile.x-(brush_size/2); x<=mouse_tile.x+(brush_size/2); x++)
						for(int y=mouse_tile.y-(brush_size/2); y<=mouse_tile.y+(brush_size/2); y++)
							stage.map[x][y] = 0;

				// CYCLE PREV TILE
				if(app.mouse.buttons[SDL_BUTTON_X1])
				{
					app.mouse.buttons[SDL_BUTTON_X1] = 0;
					IDG_CycleTile(&current_tile, -1);
				}

				// CYCLE NEXT TILE
				if(app.mouse.buttons[SDL_BUTTON_X2])
				{
					app.mouse.buttons[SDL_BUTTON_X2] = 0;
					IDG_CycleTile(&current_tile, 1);
				}

				// SELECT HOVERED TILE
				if(app.mouse.buttons[SDL_BUTTON_MIDDLE])
				{
					app.mouse.buttons[SDL_BUTTON_MIDDLE] = 0;
					if(stage.map[mouse_tile.x][mouse_tile.y] != 0)
						current_tile = stage.map[mouse_tile.x][mouse_tile.y];
				}

				break;
			case MODE_SECTOR:
				sector_2D_t *s;

				// set sector selection
				if(app.mouse.buttons[SDL_BUTTON_LEFT])
				{
					app.mouse.buttons[SDL_BUTTON_LEFT] = 0;
					IDG_Sector2DSetPoint(mouse_tile);
				}

				// open sector metadata menu
				if(app.mouse.buttons[SDL_BUTTON_RIGHT])
				{
					app.mouse.buttons[SDL_BUTTON_RIGHT] = 0;

					for(s=stage.sector2D_head.next; s!=NULL; s=s->next)
					{
						if(IDG_Collision(app.mouse.x, app.mouse.y, 1, 1, (s->x-stage.camera.pos.x), (s->y-stage.camera.pos.y), s->w, s->h))
						{
							printf("SOLID: %d | INTER: %d | LOS: %d\n", s->solid, s->interactive, s->blocks_los);
							app.dev.curr_sector_id = s->sector_id;
							app.dev.sector_menu_focused = 1;
						}
					}
				}
				break;
			case MODE_ENTITIES:
				// // ADD ENTITY
				// if(app.mouse.buttons[SDL_BUTTON_LEFT])
				// {
				// 	app.mouse.buttons[SDL_BUTTON_LEFT] = 0; // Prevent multi-place by holding button
				// 	IDG_AddEntity();
				// }
				
				// // REMOVE ENTITIY
				// if(app.mouse.buttons[SDL_BUTTON_RIGHT])
				// {
				// 	app.mouse.buttons[SDL_BUTTON_RIGHT] = 0; // Prevent multi-place by holding button
				// 	IDG_RemoveEntity();
				// }

				// // CYCLE PREV ENTITY
				// if(app.mouse.buttons[SDL_BUTTON_X1])
				// {
				// 	app.mouse.buttons[SDL_BUTTON_X1] = 0;
				// 	IDG_CycleEntity(&current_entity_index, -1);
				// 	current_entity = entities[current_entity_index];
				// }

				// // CYCLE NEXT ENTITY
				// if(app.mouse.buttons[SDL_BUTTON_X2])
				// {
				// 	app.mouse.buttons[SDL_BUTTON_X2] = 0;
				// 	IDG_CycleEntity(&current_entity_index, 1);
				// 	current_entity = entities[current_entity_index];
				// }
				break;
			case MODE_PICK:
				// SELECT ENTITY
				// if(app.mouse.buttons[SDL_BUTTON_LEFT])
				// {
				// 	app.mouse.buttons[SDL_BUTTON_LEFT] = 0;
				// 	if(current_entity == NULL)
				// 		IDG_PickEntity();
				// 	else
				// 		current_entity = NULL;
				// }
				break;
			default:
				break;
		}
	}
}

static void IDG_DoKeyboard(void)
{
	// MOVE
	int dx, dy;
	if(move_timer <= 0)
	{
		dx = dy = 0;

		if(app.keyboard[SDL_SCANCODE_A]) { dx = -MAP_TILE_SIZE; }
		if(app.keyboard[SDL_SCANCODE_D]) { dx =  MAP_TILE_SIZE; }
		if(app.keyboard[SDL_SCANCODE_W]) { dy = -MAP_TILE_SIZE; }
		if(app.keyboard[SDL_SCANCODE_S]) { dy =  MAP_TILE_SIZE; }
		
		if(dx != 0 || dy != 0)
		{
			stage.camera.pos.x = MIN(MAX((stage.camera.pos.x+dx), -SCROLL_OVERSCAN), (MAP_WIDTH*MAP_TILE_SIZE)-SCROLL_OVERSCAN);
			stage.camera.pos.y = MIN(MAX((stage.camera.pos.y+dy), -SCROLL_OVERSCAN), (MAP_HEIGHT*MAP_TILE_SIZE)-SCROLL_OVERSCAN);
			move_timer     = MOVE_MODE_SPEED;
		}
	}

	// SAVE GAME
	if(app.keyboard[SDL_SCANCODE_SPACE])
	{
		app.keyboard[SDL_SCANCODE_SPACE] = 0;
	
		IDG_SetStatusMessage("Saving map '%s' ...", stage.name);
		if(!IDG_Editor_SaveMap2D() || !IDG_SaveSector2D() /* || !IDG_SaveEntities() */)
			IDG_SetStatusMessage("Save failed.");
		else
			IDG_SetStatusMessage("Saving map '%s' ... Done", stage.name);
	}

	if(mode == MODE_SECTOR)
	{
		// CANCEL SECTOR SELECTION
		if(app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
			IDG_Sector2DResetSelection(); // reset corners
		}
	}

	// ENTER TILE MODE
	if(app.keyboard[SDL_SCANCODE_1])
	{
		app.keyboard[SDL_SCANCODE_1] = 0;
		mode = MODE_TILES;
	}

	// ENTER ENTITY MODE
	if(app.keyboard[SDL_SCANCODE_2])
	{
		app.keyboard[SDL_SCANCODE_2] = 0;
		mode = MODE_SECTOR;
	}

	// ENTER SECTOR MODE
	if(app.keyboard[SDL_SCANCODE_3])
	{
		app.keyboard[SDL_SCANCODE_3] = 0;
		mode = MODE_ENTITIES;
		// current_entity = entities[current_entity_index];
	}

	// ENTER PICK MODE
	if(app.keyboard[SDL_SCANCODE_4])
	{
		app.keyboard[SDL_SCANCODE_4] = 0;
		mode = MODE_PICK;
		// current_entity = NULL;
	}
	
	if(app.keyboard[SDL_SCANCODE_LEFTBRACKET])
	{
		app.keyboard[SDL_SCANCODE_LEFTBRACKET] = 0;
		IDG_SetBrushSize(&brush_size, -1);
	}

	if(app.keyboard[SDL_SCANCODE_RIGHTBRACKET])
	{
		app.keyboard[SDL_SCANCODE_RIGHTBRACKET] = 0;
		IDG_SetBrushSize(&brush_size, 1);
	}

	// TOGGLE SECTOR2D DRAW 
	if(app.keyboard[SDL_SCANCODE_8])
	{
		app.keyboard[SDL_SCANCODE_8] = 0;
		app.dev.show_sectors = !app.dev.show_sectors;
	}

	// TOGGLE GRID DRAW
	if(app.keyboard[SDL_SCANCODE_9])
	{
		app.keyboard[SDL_SCANCODE_9] = 0;
		app.dev.show_grid = !app.dev.show_grid;
	}

	// TOGGLE MINIMAP DRAW
	if(app.keyboard[SDL_SCANCODE_0])
	{
		app.keyboard[SDL_SCANCODE_0] = 0;
		app.dev.show_minimap = !app.dev.show_minimap;
	}
}

// static void IDG_AddEntity(void)
// {
// 	entity_t *e;

// 	// Don't re-init duplicates of unique entities
// 	if(current_entity->editor_flags & EMF_UNIQUE)
// 		e = find_existing(current_entity->type_name);
// 	else
// 		e = init_entity(current_entity->type_name);
	
// 	e->x = current_entity->x;
// 	e->y = current_entity->y;
// }

// static entity_t *find_existing(char *type_name)
// {
// 	entity_t *e;

// 	for(e=stage.entity_head.next; e!=NULL; e=e->next)
// 		if(strcmp(e->type_name, type_name) == 0)
// 			return e;
// 	return current_entity;
// }

// static void IDG_RemoveEntity(void)
// {
// 	entity_t *e, *prev;

// 	prev = &stage.entity_head;
// 	for(e=stage.entity_head.next; e!=NULL; e=e->next)
// 	{
// 		// Find the entity that overlaps the mouse pointer.
// 		// Don't allow the removal of unique things, like the player, startflag, or endflag.
// 		// TODO: allow these to be removed. Just enforce errors.
// 		if(!(e->editor_flags & EMF_UNIQUE) && collision((app.mouse.x+stage.camera.pos.x), (app.mouse.y+stage.camera.pos.y), 1, 1, e->x, e->y, e->w, e->h))
// 		{
// 			prev->next = e->next;

// 			if(e == stage.entity_tail)
// 				stage.entity_tail = prev;
			
// 			if(e->data != NULL)
// 				free(e->data);
			
// 			free(e);
// 			e = prev;
// 		}
// 		prev = e;
// 	}
// }

// static void IDG_PickEntity(void)
// {
// 	entity_t *e;
// 	for(e=stage.entity_head.next; e!=NULL; e=e->next)
// 	{
// 		if(collision((app.mouse.x+stage.camera.pos.x), (app.mouse.y+stage.camera.pos.y), 1, 1, e->x, e->y, e->w, e->h))
// 		{
// 			current_entity = e;
// 			return;
// 		}
// 	}
// }

static void IDG_CycleTile(int *i, int dir)
{
	do
	{
		*i = *i + dir;
		if(*i < 0) { *i = (MAX_TILES-1); }
		if(*i >= MAX_TILES) { *i = 1; }
	} while(tiles[*i] == NULL);
}

// static void IDG_CycleEntity(int *i, int dir)
// {
// 	*i = *i + dir;
// 	if(*i < 0) { *i = (total_entities-1); }
// 	if(*i >= total_entities) { *i = 0; }
// }

static void IDG_DrawEditor2DUI(void)
{
	IDG_DrawEditor2DTopBar    	();
	IDG_DrawEditor2DBottomBar 	();
	if(app.dev.show_minimap)
		IDG_DrawEditor2DMinimap ();
	IDG_DrawStatusMessage       ();
}

static void IDG_DrawEditor2DTopBar(void)
{
	char text[MAX_LINE_LENGTH];

	IDG_DrawRect(0, 0, SCREEN_WIDTH, 36, 0, 0, 0, 192);

	app.font_scale = 0.75;
	if(mode == MODE_TILES || mode == MODE_SECTOR)
		sprintf(text, "%d,%d", mouse_tile.x, mouse_tile.y);
	else if(current_entity != NULL)
		sprintf(text, "%d,%d", (int)current_entity->x, (int)current_entity->y);
	else
		sprintf(text, "-,-");
	IDG_DrawText(text, 10, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	sprintf(text, "Mode: %s", mode_text[mode]);
	IDG_DrawText(text, 100, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	if(mode == MODE_TILES)
	{
		sprintf(text, "Brush Size: %d", brush_size);
		IDG_DrawText(text, 250, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

		sprintf(text, "( [ ] )");
		IDG_DrawText(text, 400, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);
	}

	// if(mode == MODE_TILES)
	// {
	// 	sprintf(text, "Tile type: %s", IDG_GetTileTypeName());
	// 	draw_text(text, 800, 0, 255, 255, 255, IDG_FindExisting, 0);
	// }
	// else if(current_entity != NULL)
	// {
	// 	sprintf(text, "Entity: %s", current_entity->type_name);
	// 	draw_text(text, 800, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);
	// }

	sprintf(text, "Map: %s", stage.name);
	IDG_DrawText(text, (SCREEN_WIDTH-10), 0, 255, 255, 255, TEXT_ALIGN_RIGHT, 0);
	app.font_scale = 1.0;
}

static void IDG_DrawEditor2DBottomBar(void)
{
	const int bar_y = (SCREEN_HEIGHT-MAP_TILE_SIZE-BOTTOM_HUD_PADDING); 
	int       x, x2, i, j;

	IDG_DrawRect(		 // Draw tile (or entity) selector background
		0, 				 // x
		bar_y,	         // y
		SCREEN_WIDTH,	 // w
		bar_y, 	         // h
		32, 32, 32, 255	 // rbga
	);

	x  = (SCREEN_WIDTH-MAP_TILE_SIZE)/2;
	x2 = x-(MAP_TILE_SIZE+4);

	switch(mode)
	{
		case MODE_TILES:
			i = current_tile;
			j = current_tile;

			IDG_CycleTile(&j, -1);

			while(x < SCREEN_WIDTH)
			{
				IDG_BlitAtlasImage(tiles[i], x, bar_y, 0, SDL_FLIP_NONE);
				IDG_BlitAtlasImage(tiles[j], x2, bar_y, 0, SDL_FLIP_NONE);
			
				x  += MAP_TILE_SIZE + 4;
				x2 -= MAP_TILE_SIZE + 4;

				IDG_CycleTile(&i, 1);
				IDG_CycleTile(&j, -1);
			} 

			break;
		case MODE_ENTITIES:
			// i = current_entity_index;
			// j = current_entity_index;
			
			// IDG_CycleEntity(&j, -1);

			// while(x < SCREEN_WIDTH)
			// {
			// 	blit_atlas_image(entities[i]->texture, x, (SCREEN_HEIGHT-50), 0, SDL_FLIP_NONE);
			// 	blit_atlas_image(entities[j]->texture, x2, (SCREEN_HEIGHT-50), 0, SDL_FLIP_NONE);

			// 	x  += MAP_TILE_SIZE + 4;
			// 	x2 -= MAP_TILE_SIZE + 4;

			// 	IDG_CycleEntity(&i, 1);
			// 	IDG_CycleEntity(&j, -1);
			// }

			break;
		default:
			break;
	}

	if(mode != MODE_PICK)
	{
		x = (SCREEN_WIDTH-MAP_TILE_SIZE)/2;

		IDG_DrawOutlineRect (x, (SCREEN_HEIGHT-MAP_TILE_SIZE-2), MAP_TILE_SIZE, MAP_TILE_SIZE, 255, 255, 0, 255);
	}
}

static void IDG_DrawEditor2DMinimap(void)
{
	entity_t *e;
	int       x, y;

	IDG_DrawRect(minimap_rect.x, minimap_rect.y, minimap_rect.w, minimap_rect.h, 0, 0, 0, 255);

	for(x=0; x<MAP_WIDTH; x++)
	{
		for(y=0; y<MAP_HEIGHT; y++)
		{
			if(IDG_Editor_IsSolidMap(x, y))
				IDG_DrawRect((minimap_rect.x+x*MINIMAP_CEL_SIZE), (minimap_rect.y+y*MINIMAP_CEL_SIZE), MINIMAP_CEL_SIZE, MINIMAP_CEL_SIZE, 96, 96, 96, 255);
			// else if(is_water(x, y))
			else
				IDG_DrawRect((minimap_rect.x+x*MINIMAP_CEL_SIZE), (minimap_rect.y+y*MINIMAP_CEL_SIZE), MINIMAP_CEL_SIZE, MINIMAP_CEL_SIZE, 128, 128, 255, 255);
		}
	}

	// for(e=stage.entity_head.next; e!=NULL; e=e->next)
	// {
	// 	x = (e->x/MAP_TILE_SIZE);
	// 	y = (e->y/MAP_TILE_SIZE);
	// 	IDG_DrawRect((minimap_rect.x+x*MINIMAP_CEL_SIZE), (minimap_rect.y+y*MINIMAP_CEL_SIZE), MINIMAP_CEL_SIZE, MINIMAP_CEL_SIZE, 255, 255, 0, 255);
	// }

	IDG_DrawOutlineRect(minimap_rect.x, minimap_rect.y, minimap_rect.w, minimap_rect.h, 255, 255, 255, 255);
}

static void IDG_DrawStatusMessage(void)
{
	if(status_message_timer > 0)
	{
		app.font_scale = 0.75;
		IDG_DrawText(status_message, (SCREEN_WIDTH-200), 0, 255, 255, 255, TEXT_ALIGN_CENTER, 0);
		app.font_scale = 1.0;
	}
}

static void IDG_SetStatusMessage(char *format, ...)
{
	va_list args;

	memset(&status_message, '\0', sizeof(status_message));

	va_start(args, format);
	vsprintf(status_message, format, args);
	va_end(args);

	status_message_timer = (FPS*1.5);
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%s", status_message);
}

// static char *IDG_GetTileTypeName(void)
// {
// 	// There's gotta be a better way to do this...
// 	if(current_tile >= TILE_WATER)
// 		return "Water";
// 	else if(current_tile >= TILE_FOREGROUND)
// 		return "Foreground";
// 	else
// 		return "Solid";
// }

// *** TODO ***
// Duplicate of IDG_LoadTiles in IDG_2DMap.c
// Make a fuckin editor utilities file...
static void IDG_LoadTiles(void)
{
	int  i;
	char filename[MAX_FILENAME_LENGTH];

	for (i=0; i<MAX_TILES; i++)
	{
		// *** TODO ***
		// For now, this is fine for personal use...
		// Ideally this directory should be read from cmdline args and loaded without the need for indexes.
		// but uuuughhhhhhhhhh. i dunno. reference IDG_AtlasGen.c.
		// 
		// Also should probably have better error handling for overflowing MAX_TILES

		sprintf(filename, "data/gfx/tiles/tile%d.png", i);
		tiles[i] = IDG_GetAtlasImage(filename, 0);
	}
}

static void IDG_SetupWidgets(void)
{
	// TODO - persist sw->value from JSON
	widget_t        *w;
	select_widget_t *sw;
	sector_2D_t     *s;

	w = IDG_GetWidget("interactive", "sectors");
	w->action = IDG_Sector2DSetInteractive;
	sw = (select_widget_t *)w->data;
	sw->value = 0;

	w = IDG_GetWidget("solid", "sectors");
	w->action = IDG_Sector2DSetSolid;
	sw = (select_widget_t *)w->data;
	sw->value = 0;

	w = IDG_GetWidget("blocks_los", "sectors");
	w->action = IDG_Sector2DSetBlocksLOS;
	sw = (select_widget_t *)w->data;
	sw->value = 0;

	w = IDG_GetWidget("delete", "sectors");
	w->action = IDG_DeleteSector2D;
}