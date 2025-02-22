#include <SDL2/SDL_image.h>

#include "../IDG_Common.h"

#include "IDG_Textures.h"

extern app_t app;

static texture_t  texture_head, *texture_tail;

void IDG_InitTextures(void)
{
    memset(&texture_head, 0, sizeof(texture_t));
	texture_tail = &texture_head;
}

static SDL_Texture *IDG_GetTexture(char *name)
{
    texture_t *t;
	for (t = texture_head.next; t != NULL; t = t->next)
		if (strcmp(t->name, name) == 0)
			return t->texture;
	return NULL;
}

// *** TODO ***
// Why does this crash...
static void IDG_CacheTexture(char *name, SDL_Texture *sdl_texture)
{
	texture_t *texture;

	texture = malloc(sizeof(texture_t));
	memset(texture, 0, sizeof(texture_t));
	texture_tail->next = texture;
	texture_tail       = texture;

	STRNCPY(texture->name, name, MAX_FILENAME_LENGTH);
	texture->texture = sdl_texture;
}

SDL_Texture *IDG_ToTexture(SDL_Surface *surface, int destroy_surface)
{
    SDL_Texture *texture;

	texture = SDL_CreateTextureFromSurface(app.renderer, surface);
	if (destroy_surface)
		SDL_FreeSurface(surface);
	return texture;
}

SDL_Texture *IDG_LoadTexture(char *filename)
{
    SDL_Texture *texture;

	texture = IDG_GetTexture(filename);
	if (texture == NULL)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s ...", filename);
		texture = IMG_LoadTexture(app.renderer, filename);
		// IDG_CacheTexture(filename, texture);
	}
	return texture;
}