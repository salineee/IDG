#ifndef __IDG_DRAW__
#define __IDG_DRAW__

void IDG_InitDraw        (void);
void IDG_PrepareScene    (void);
void IDG_PresentScene    (void);

/* Rendering functions */
void IDG_Blit            (SDL_Texture *texture, int x, int y, int center);
void IDG_BlitAtlasImage  (atlas_image_t *atlas_image, int x, int y, int center, SDL_RendererFlip flip);
void IDG_BlitRotated     (atlas_image_t *atlas_image, int x, int y, double angle);
void IDG_BlitScaled      (atlas_image_t *atlas_image, int x, int y, int w, int h, int center);

void IDG_DrawRect        (int x, int y, int w, int h, int r, int g, int b, int a);
void IDG_DrawOutlineRect (int x, int y, int w, int h, int r, int g, int b, int a);
void IDG_DrawGridLines   (int spacing);

#endif // __IDG_DRAW__