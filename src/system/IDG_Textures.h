#ifndef __IDG_TEXTURES__
#define __IDG_TEXTURES__

void         IDG_InitTextures (void);
SDL_Texture *IDG_ToTexture    (SDL_Surface *surface, int destroy_surface);
SDL_Texture *IDG_LoadTexture  (char *filename);

#endif // __IDG_TEXTURES__