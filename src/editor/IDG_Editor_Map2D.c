#include "../IDG_Common.h"

#include "../system/IDG_Atlas.h"
#include "../system/IDG_Draw.h"
#include "../system/IDG_Textures.h"
#include "../system/IDG_Util.h"

#include "IDG_Editor_Map2D.h"

#define MAP_DATA_FILENAME_LENGTH (MAX_FILENAME_LENGTH+4)

extern app_t   app;
extern stage_t stage;

static void  IDG_Editor_LoadTiles(void);
static char *IDG_Editor_GetMapFilename(void);

static atlas_image_t *tiles[MAX_TILES];
static atlas_image_t *checkerboard_texture;

void IDG_Editor_InitMap2D(void)
{
    IDG_Editor_LoadTiles();
    checkerboard_texture = IDG_GetAtlasImage("data/gfx/system/transparent_checkerboard.png", 1);
    IDG_Editor_LoadMap2D();
}

int IDG_Editor_IsInsideMap2D(int x, int y)
{
    return x>=0 && y>=0 && x<MAP_WIDTH && y<MAP_HEIGHT;
}

int IDG_Editor_IsSolidMap(int x, int y)
{
    return stage.map[x][y] >= TILE_SOLID && stage.map[x][y] < TILE_FOREGROUND;
}

void IDG_Editor_LoadMap2D(void)
{
    char *data, *p, *filename;
    int   x, y;

    filename = IDG_Editor_GetMapFilename();
    if(IDG_FileExists(filename))
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading map %s ...\nAborting ...", filename);

        data = IDG_ReadFile(filename);
        p    = data;

        for(y=0; y<MAP_HEIGHT; y++)
        {
            for(x=0; x<MAP_WIDTH; x++)
            {
                stage.map[x][y] = atoi(p);
                do { p++; } while(isdigit(*p));
            }
        }
        free(data);
    }
    else if(app.dev.is_editor)
    {
        // TODO - this should be extracted to check for a valid map, and create a new one,
        //        before initializing the renderer and opening the window
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Couldn't load '%s', file does not exist.", filename);
        
        char yn;
        printf("\aINFO: Create '%s'? (Y/N) ", filename);
        scanf("%c", &yn);
        switch(tolower(yn))
        {
            case 'y':
                SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Creating new file '%s.map'", filename);
                break;
            default:
                SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Couldn't load '%s' ...\nAborting ...", filename);
                exit(1);
                break;
        }
    }
    else
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Couldn't load '%s' ...\n", filename);
        free(filename);
        exit(1);
    }
    free(filename);
}

void IDG_Editor_DrawMap2D(int layer)
{
    int x, y, n, x1, x2, y1, y2, mx, my;

    x1 = ((int)stage.camera.pos.x % MAP_TILE_SIZE) * -1;
    x2 = x1 + MAP_RENDER_WIDTH * MAP_TILE_SIZE + (x1==0 ? 0 : MAP_TILE_SIZE);

    y1 = ((int)stage.camera.pos.y % MAP_TILE_SIZE) * -1;
    y2 = y1 + MAP_RENDER_HEIGHT * MAP_TILE_SIZE + (y1==0 ? 0 : MAP_TILE_SIZE);

    mx = (stage.camera.pos.x/MAP_TILE_SIZE);
    my = (stage.camera.pos.y/MAP_TILE_SIZE);

    for(y=y1; y<y2; y+=MAP_TILE_SIZE)
    {
        for(x=x1; x<x2; x+=MAP_TILE_SIZE)
        {
            if(IDG_Editor_IsInsideMap2D(mx, my))
            {
                n = stage.map[mx][my];
                if((n > 0) && (layer == LAYER_BACKGROUND || layer == LAYER_FOREGROUND))
                {
                    IDG_BlitAtlasImage(tiles[n], x, y, 0, SDL_FLIP_NONE);
                }
            }
            else if(app.dev.is_editor)
            {
                IDG_BlitAtlasImage(checkerboard_texture, x, y, 0, SDL_FLIP_NONE);
            }
            mx++;
        }
        mx = (stage.camera.pos.x/MAP_TILE_SIZE);
        my++;
    }    
}

// *** TODO ***
// Duplicate of IDG_Editor_LoadTiles in IDG_Editor2D.c
// Make a fuckin editor utilities file...
static void IDG_Editor_LoadTiles(void)
{
    int  i;
    char filename[MAX_FILENAME_LENGTH];

    for(i=0; i<MAX_TILES; i++)
    {
        sprintf(filename, "data/gfx/tiles/tile%d.png", i);
        tiles[i] = IDG_GetAtlasImage(filename, 0);
    }
}

static char *IDG_Editor_GetMapFilename(void)
{
    return IDG_FormattedString("data/stages/%s.map", stage.name);
}

int IDG_Editor_SaveMap2D(void)
{
    FILE *fp;
    char *filename;
    int   x, y, rtn;

    filename = IDG_Editor_GetMapFilename();
	fp = fopen(filename, "w");
	if (fp)
	{
		for (y=0; y<MAP_HEIGHT; y++)
			for (x=0; x<MAP_WIDTH; x++)
				fprintf(fp, "%d ", stage.map[x][y]);

		fclose(fp);
		rtn = 1;
	}
	else
	{
		rtn = 0;
	}

	free(filename);
	return rtn;
}