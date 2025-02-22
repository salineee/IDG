#ifndef __IDG_WIDGETS__
#define __IDG_WIDGETS__

void      IDG_InitWidgets (void);
void      IDG_DoWidgets   (char *group_name);
void      IDG_DrawWidgets (char *group_name);
widget_t *IDG_GetWidget   (char *name, char *group_name);

#endif // __IDG_WIDGETS__