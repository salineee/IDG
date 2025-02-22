#ifndef __IDG_EDITOR_MAP2D__
#define __IDG_EDITOR_MAP2D__

void IDG_Editor_InitMap2D     (void);
int  IDG_Editor_IsInsideMap2D (int x, int y);
int  IDG_Editor_IsSolidMap    (int x, int y);
void IDG_Editor_LoadMap2D     (void);
int  IDG_Editor_SaveMap2D     (void);
void IDG_Editor_DrawMap2D     (int layer);

#endif // __IDG_EDITOR_MAP2D__