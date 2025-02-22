#ifndef __IDG_UTIL__
#define __IDG_UTIL__

int           IDG_Collision       (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
int           IDG_RectCollide     (SDL_Rect *r1, SDL_Rect *r2);
int           IDG_SphCollide      (int d,  int r1, int r2);
int           IDG_LineCollide     (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int           IDG_LineRectCollide (int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh);

void          IDG_GetSlope        (int x1, int y1, int x2, int y2, double *dx, double *dy);
double        IDG_GetAngle        (int x1, int y1, int x2, int y2);
int           IDG_GetDistance     (int x1, int y1, int x2, int y2);

unsigned long IDG_Hashcode        (const char *str);
void         *IDG_ResizeArray     (void *array, int curr_size, int target_size);

// *** TODO ***
// Move file utils to a separate file
int           IDG_FileExists      (const char *filename);
char         *IDG_ReadFile        (char *filename);
int           IDG_WriteFile       (const char *filename, const char *data);
char         *IDG_FormattedString (const char *format, ...);

#endif // __IDG_UTIL__