#include <SDL2/SDL_image.h>

#include "../IDG_Common.h"

#include "../json/cJSON.h"
#include "../system/IDG_Util.h"

#include "IDG_Atlas.h"

extern app_t app;

static void IDG_LoadAtlasData(void);

static atlas_image_t atlases[NUM_ATLAS_BUCKETS];
static SDL_Texture  *atlas_texture;

void IDG_InitAtlas(void)
{
    memset(&atlases, 0, sizeof(atlas_image_t) * NUM_ATLAS_BUCKETS);
	atlas_texture = IMG_LoadTexture(app.renderer, "data/atlas/atlas.png");
	IDG_LoadAtlasData();
}

atlas_image_t *IDG_GetAtlasImage(char *filename, int required)
{
    atlas_image_t *a;
    unsigned long  i;

    i = IDG_Hashcode(filename) % NUM_ATLAS_BUCKETS;
	for (a = atlases[i].next; a != NULL; a = a->next)
	{
		if (strcmp(a->filename, filename) == 0)
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Fetching Atlas Image '%s' ... \n", filename);
			return a;
		}
	}

	if (required)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such atlas image '%s'", filename);
		exit(1);
	}

	return NULL;
}

static void IDG_LoadAtlasData(void)
{
    atlas_image_t *atlas_image, *a;
	cJSON         *root, *node;
	char		  *text, *filename;
	int            x, y, w, h, rotated;
	unsigned long  i;

	text = IDG_ReadFile("data/atlas/atlas.json");
	root = cJSON_Parse(text);

	for (node = root->child; node != NULL; node = node->next)
	{
		filename = cJSON_GetObjectItem(node, "filename")->valuestring;
		x = cJSON_GetObjectItem(node, "x")->valueint;
		y = cJSON_GetObjectItem(node, "y")->valueint;
		w = cJSON_GetObjectItem(node, "w")->valueint;
		h = cJSON_GetObjectItem(node, "h")->valueint;
		rotated = cJSON_GetObjectItem(node, "rotated")->valueint;

		i = IDG_Hashcode(filename) % NUM_ATLAS_BUCKETS;
		a = &atlases[i];

		/* horrible bit to look for the tail */
		while (a->next)
			a = a->next;

		atlas_image = malloc(sizeof(atlas_image_t));
		memset(atlas_image, 0, sizeof(atlas_image_t));
		a->next = atlas_image;

		STRNCPY(atlas_image->filename, filename, MAX_FILENAME_LENGTH);
		atlas_image->rect.x  = x;
		atlas_image->rect.y  = y;
		atlas_image->rect.w  = w;
		atlas_image->rect.h  = h;
		atlas_image->rotated = rotated;

		atlas_image->texture = atlas_texture;
	}

	cJSON_Delete(root);

	free(text);
}