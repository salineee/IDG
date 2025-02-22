#ifndef __IDG_BRUSH__
#define __IDG_BRUSH__

void IDG_SetBrushSize (int *brush_size, int dir);
void IDG_DrawBrush    (int brush_size, SDL_Point mouse_tile);

#endif // __IDG_BRUSH__