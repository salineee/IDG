#ifndef __IDG_TEXT__
#define __IDG_TEXT__

void         IDG_InitFonts            (void);
SDL_Texture *IDG_GetTextTexture       (char *text, int type);
void         IDG_DrawText             (char *text, int x, int y, int r, int g, int b, int align, int max_width);
void         IDG_GetTextDimensions    (char *text, int *w, int *h);
int          IDG_GetWrappedTextHeight (char *text, int max_width);

#endif // __IDG_TEXT__