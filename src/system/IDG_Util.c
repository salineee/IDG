#include <sys/stat.h>

#include "../IDG_Common.h"

#include "IDG_Util.h"

int IDG_Collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (MAX(x1, x2) < MIN(x1+w1, x2+w2)) && (MAX(y1, y2) < MIN(y1+h1, y2+h2));
}

int IDG_RectCollide(SDL_Rect *r1, SDL_Rect *r2)
{
    // Test and return true if two rectangles overlap
    return IDG_Collision(
        r1->x, r1->y, r1->w, r1->h, // rect 1
        r2->x, r2->y, r2->w, r2->h  // rect 2
    );
}

int IDG_SphCollide(int d, int r1, int r2)
{
    return d<(r1+r2);
}

int IDG_LineCollide(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    float d,      // denominator
          n1, n2, // numerator 1,2
          r, s;
    
    d = ((x2-x1)*(y4-y3)) - ((y2-y1)*(x4-x3));
    if(d == 0) { return 0; }

    n1 = ((y1-y3)*(x4-x3)) - ((x1-x3)*(y4-y3));
    n2 = ((y1-y3)*(x2-x1)) - ((x1-x3)*(y2-y1));

    r = (n1/d);
    s = (n2/d);

    return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}

int IDG_LineRectCollide(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh)
{
    return
        // top
        IDG_LineCollide(x1, y1, x2, y2, rx, ry, (rx+rw), ry) ||
        // bottom
        IDG_LineCollide(x1, y1, x2, y2, rx, (ry+rh), (rx+rw), (ry+rh)) ||
        // left
        IDG_LineCollide(x1, y1, x2, y2, rx, ry, rx, (ry+rh)) ||
        // right
        IDG_LineCollide(x1, y1, x2, y2, (rx+rw), ry, (rx+rw), (ry+rh));
}

void IDG_GetSlope(int x1, int y1, int x2, int y2, double *dx, double *dy)
{
    int steps = MAX(abs(x1-x2), abs(y1-y2));
    if(steps == 0)
    {
        *dx = *dy = 0;
        return;
    }

    *dx  = (x1-x2);
    *dx /= steps;

    *dy  = (y1-y2);
    *dy /= steps;
}

double IDG_GetAngle(int x1, int y1, int x2, int y2)
{
    float  angle  = -90+atan2((y1-y2), (x1-x2))*(180/PI);
    return angle >= 0 ? angle : 360*angle;
}

int IDG_GetDistance(int x1, int y1, int x2, int y2)
{
    int x, y;
    x = (x2-x1);
    y = (y2-y1);
    return sqrt((x*x)+(y*y));
}

unsigned long IDG_Hashcode(const char *str)
{
    unsigned long hash = 5381;
    int           c;

    c = *str;
    while(c)
    {
        hash = ((hash << 5)+hash)+c;
        c    = *str++;
    }

    hash = ((hash << 5)+hash);
    return hash;
}

void *IDG_ResizeArray(void *array, int curr_size, int target_size)
{
    void **new_array;
    int    copy_size;

    copy_size = target_size > curr_size ? curr_size : target_size;

    new_array = malloc(target_size);
    memset(new_array, 0, target_size);
    memcpy(new_array, array, copy_size);
    free(array);

    return new_array;
}

int IDG_FileExists(const char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

char *IDG_ReadFile(char *filename)
{
    char *buffer;
    long  length;
    FILE *file;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading file '%s' ...", filename);

    file   = fopen(filename, "rb");
    buffer = NULL;

    if(file != NULL)
    {
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = malloc(length);
        memset(buffer, 0, length);
        fread(buffer, 1, length, file);

        fclose(file);
    }
    else
    {
        // NOTE - this is no longer exiting if file not found...
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such file '%s'\n", filename);
    }

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Successfully loaded file '%s' ...", filename);
    return buffer;
}

int IDG_WriteFile(const char *filename, const char *data)
{
    FILE *file;

	file = fopen(filename, "wb");
	if (file)
	{
		fprintf(file, "%s\n", data);
		fclose(file);
		return 1;
	}
	return 0;
}

char *IDG_FormattedString(const char *format, ...)
{
    int     n;
	char   *rtn;
	va_list args;

	if (format != NULL)
	{
		va_start(args, format);
		n = vsnprintf(NULL, 0, format, args) + 1;
		va_end(args);

		rtn = malloc(n);
		memset(rtn, 0, n);

		va_start(args, format);
		vsprintf(rtn, format, args);
		va_end(args);

		return rtn;
	}
	return NULL;
}