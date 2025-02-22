#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Textures.h"
#include "../system/IDG_Util.h"

#include "IDG_Map2D.h"

extern app_t   app;
extern stage_t stage;

static void IDG_LoadMapTiles (void);
static void IDG_LoadMapData  (char *filename);

static atlas_image_t *tiles[MAX_TILES];

void IDG_InitMap2D(void)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Initializing Map2D as %s", (app.dev.is_editor ? "EDITOR" : "STAGE"));

	char filename[MAX_FILENAME_LENGTH];
	sprintf(filename, "data/stages/%s.map", stage.name);

    IDG_LoadMapTiles ();
    IDG_LoadMapData  (filename);
}

void IDG_DrawMap2D(void)
{
	int x, y, n;

	// TODO - extract each case into private functions
	switch (stage.camera.initialized)
	{
		case 1: /* camera initialized */
			int x1, x2, y1, y2, mx, my;

			x1 = (stage.camera.pos.x%MAP_TILE_SIZE)*-1;
			x2 = x1 + MAP_RENDER_WIDTH * MAP_TILE_SIZE + (x1 == 0 ? 0 : MAP_TILE_SIZE);

			y1 = (stage.camera.pos.y%MAP_TILE_SIZE)*-1;
			y2 = y1 + MAP_RENDER_HEIGHT * MAP_TILE_SIZE + (y1 == 0 ? 0 : MAP_TILE_SIZE);

			mx = (stage.camera.pos.x/MAP_TILE_SIZE);
			my = (stage.camera.pos.y/MAP_TILE_SIZE);

			for (y = y1; y < y2; y += MAP_TILE_SIZE)
			{
				for (x=x1; x<x2; x+=MAP_TILE_SIZE)
				{
					if (IDG_IsInsideMap2D(mx, my))
					{
						n = stage.map[mx][my];
						if (n > 0)
						{
							IDG_BlitAtlasImage(tiles[n], x, y, 0, SDL_FLIP_NONE);
						}
					}
					mx++;
				}
				mx = stage.camera.pos.x / MAP_TILE_SIZE;
				my++;
			}
			break;
		default:
			for(x=0; x<MAP_WIDTH; x++)
			{
				for(y=0; y<MAP_HEIGHT; y++)
				{
					n = stage.map[x][y];
					if(n>0)
					{
						IDG_BlitAtlasImage(tiles[n], (x*MAP_TILE_SIZE), (y*MAP_TILE_SIZE), 0, SDL_FLIP_NONE);
					}
				}
			}
			break;
	}

}

int IDG_IsInsideMap2D(int x, int y)
{
	return x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT;
}

static void IDG_LoadMapTiles(void)
{
	char filename[MAX_FILENAME_LENGTH];
	int  i;

	for (i=1; i<MAX_TILES; i++)
	{
		sprintf(filename, "data/gfx/tiles/tile%d.png", i);
		tiles[i] = IDG_GetAtlasImage(filename, 0);
	}
}

static void IDG_LoadMapData(char *filename)
{
	char *data, *p;
	int   x, y;

	data = IDG_ReadFile(filename);
	p    = data;

	for (y=0; y<MAP_HEIGHT; y++)
	{
		for (x=0; x<MAP_WIDTH; x++)
		{
			stage.map[x][y] = atoi(p);

			do { p++; } while (isdigit(*p));
		}
	}
	free(data);
}