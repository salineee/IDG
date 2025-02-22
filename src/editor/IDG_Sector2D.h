#ifndef __IDG_SECTOR2D__
#define __IDG_SECTOR2D__

void         IDG_InitSector2D                  (void);
void         IDG_DrawSector2D                  (void);
void         IDG_Sector2DDrawSelectionPreview  (SDL_Point mouse_tile);
void         IDG_Sector2DDrawHoverState        (void);

void         IDG_Sector2DResetSelection        (void);
void         IDG_Sector2DSetPoint              (SDL_Point mouse_tile);
void         IDG_DeleteSector2D                (void);

void         IDG_Sector2DSetBlocksLOS          (void);
void         IDG_Sector2DSetSolid              (void);
void         IDG_Sector2DSetInteractive        (void);

void         IDG_LoadSector2D                  (void);
int          IDG_SaveSector2D                  (void);

#endif // __IDG_SECTOR2D__