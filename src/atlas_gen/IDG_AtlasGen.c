#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* define glibc extension, to use DT_DIR */
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <dirent.h>
#include <math.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../json/cJSON.h"
#include "../IDG_Defs.h"
#include "../IDG_Structs.h"

static image_t *images;
static int      atlas_size;
static int      padding;
static char    *root_dir;

static void IDG_SplitNode(Node *node, int w, int h)
{
	node->used = 1;

	node->child = malloc(sizeof(Node) * 2);
	memset(node->child, 0, sizeof(Node) * 2);

	node->child[0].x = node->x + w + padding;
	node->child[0].y = node->y;
	node->child[0].w = node->w - w - padding;
	node->child[0].h = h;

	node->child[1].x = node->x;
	node->child[1].y = node->y + h + padding;
	node->child[1].w = node->w;
	node->child[1].h = node->h - h - padding;
}

static Node *IDG_FindNode(Node *root, int w, int h)
{
	if (root->used)
	{
		Node *n = NULL;
		if ((n = IDG_FindNode(&root->child[0], w, h)) != NULL || (n = IDG_FindNode(&root->child[1], w, h)) != NULL)
			return n;
	}
	else if (w <= root->w && h <= root->h)
	{
		IDG_SplitNode(root, w, h);
		return root;
	}
	return NULL;
}

static void IDG_FreeNode(Node *node)
{
	if (node->used)
	{
		IDG_FreeNode(&node->child[0]);
		IDG_FreeNode(&node->child[1]);
		free(node->child);
	}
}

static int IDG_CountImages(const char *dir)
{
	DIR		      *d;
	struct dirent *ent;
	char		  *path;
	int            i;

	i = 0;

	if ((d = opendir(dir)) != NULL)
	{
		while ((ent = readdir(d)) != NULL)
		{
			if (ent->d_type == DT_DIR)
			{
				if (ent->d_name[0] != '.')
				{
					path = malloc(strlen(dir) + strlen(ent->d_name) + 2);
					sprintf(path, "%s/%s", dir, ent->d_name);
					i += IDG_CountImages(path);
					free(path);
				}
			}
			else
			{
				i++;
			}
		}
		closedir(d);
	}
	return i;
}

static void IDG_LoadImageData(int *i, const char *dir)
{
	DIR		      *d;
	struct dirent *ent;
	char		  *path;

	if ((d = opendir(dir)) != NULL)
	{
		while ((ent = readdir(d)) != NULL)
		{
			path = malloc(strlen(dir) + strlen(ent->d_name) + 2);
			if (ent->d_type == DT_DIR)
			{
				if (ent->d_name[0] != '.')
				{
					sprintf(path, "%s/%s", dir, ent->d_name);
					IDG_LoadImageData(i, path);
				}
			}
			else
			{
				sprintf(path, "%s/%s", dir, ent->d_name);
				images[*i].surface = IMG_Load(path);

				if (images[*i].surface)
				{
					images[*i].filename = malloc(strlen(path) + 1);

					strcpy(images[*i].filename, path);
					SDL_SetSurfaceBlendMode(images[*i].surface, SDL_BLENDMODE_NONE);
					*i = *i + 1;
				}
			}
			free(path);
		}
		closedir(d);
	}
}

static int IDG_ImageComparator(const void *a, const void *b)
{
	image_t *i1 = (image_t *)a;
	image_t *i2 = (image_t *)b;
	return i2->surface->h - i1->surface->h;
}

static void IDG_HandleCmdLine(int argc, char *argv[])
{
	int i;

	/* defaults */
	atlas_size = 256;
	root_dir   = "gfx";
	padding    = 1;

	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--size") == 0 || strcmp(argv[i], "-S") == 0)
			atlas_size = atoi(argv[i + 1]);
		else if (strcmp(argv[i], "--dir") == 0 || strcmp(argv[i], "-D") == 0)
			root_dir = argv[i + 1];
		else if (strcmp(argv[i], "--padding") == 0 || strcmp(argv[i], "-P") == 0)
			padding = atoi(argv[i + 1]);
	}
}

static int IDG_InitImages(void)
{
	int i;

	i	   = IDG_CountImages(root_dir);
	images = malloc(sizeof(image_t) * i);
	memset(images, 0, sizeof(image_t) * i);
	i = 0;

	IDG_LoadImageData(&i, root_dir);
	qsort(images, i, sizeof(image_t), IDG_ImageComparator);

	return i;
}

static void IDG_PutPixel(int x, int y, Uint32 pixel, SDL_Surface *dest)
{
	int    bpp;
	Uint8 *p;

	bpp = dest->format->BytesPerPixel;
	p = (Uint8 *)dest->pixels + y * dest->pitch + x * bpp;

	switch (bpp)
	{
		case 1:
			*p = pixel;
			break;
		case 2:
			*(Uint16 *)p = pixel;
			break;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else
			{
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}

static int IDG_GetPixel(SDL_Surface *surface, int x, int y)
{
	int    bpp;
	Uint8 *p;

	bpp = surface->format->BytesPerPixel;
	p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
		case 1:
			return *p;
		case 2:
			return *(Uint16 *)p;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;
		case 4:
			return *(Uint32 *)p;
		default:
			return 0;
	}
}

static void IDG_Atlas_BlitRotated(SDL_Surface *src, SDL_Surface *dest, int dest_x, int dest_y)
{
	int x, y, p, dx, dy;
	dy = 0;
	for(x=0; x<src->w; x++)
	{
		dx = src->h - 1;
		for (y = 0; y < src->h; y++)
		{
			p = IDG_GetPixel(src, x, y);
			IDG_PutPixel((dest_x+dx), (dest_y+dy), p, dest);
			dx--;
		}
		dy++;
	}
}

int main(int argc, char *argv[])
{
	Node        *root, *n;
	int          w, h, rotated, i, fails, rotations, num_images;
	SDL_Surface *atlas;
	SDL_Rect     dest;
	cJSON       *rootJSON, *entryJSON;
	char        *out;
	FILE        *fp;

	IDG_HandleCmdLine(argc, argv);

	SDL_Init(SDL_INIT_VIDEO);

	root       = malloc(sizeof(Node));
	root->x    = 0;
	root->y    = 0;
	root->w    = atlas_size;
	root->h    = atlas_size;
	root->used = 0;

	fails = rotations = 0;

	num_images = IDG_InitImages();
	atlas      = SDL_CreateRGBSurface(0, atlas_size, atlas_size, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	rootJSON   = cJSON_CreateArray();

	for (i=0; i<num_images; i++)
	{
		rotated = 0;

		w = images[i].surface->w;
		h = images[i].surface->h;
		n = IDG_FindNode(root, w, h);

		if (n == NULL)
		{
			rotated = 1;
			n       = IDG_FindNode(root, h, w);
		}

		if (n != NULL)
		{
			if (rotated)
			{
				n->h = w;
				n->w = h;
				rotations++;
			}

			dest.x = n->x;
			dest.y = n->y;
			dest.w = n->w;
			dest.h = n->h;

			if (!rotated)
				SDL_BlitSurface(images[i].surface, NULL, atlas, &dest);
			else
				IDG_Atlas_BlitRotated(images[i].surface, atlas, dest.x, dest.y);

			printf("[%04d / %04d] %s\n", i + 1, num_images, images[i].filename);

			entryJSON = cJSON_CreateObject();

			cJSON_AddStringToObject ( entryJSON, "filename", images[i].filename );
			cJSON_AddNumberToObject ( entryJSON, "x",        dest.x             );
			cJSON_AddNumberToObject ( entryJSON, "y",        dest.y             );
			cJSON_AddNumberToObject ( entryJSON, "w",        dest.w             );
			cJSON_AddNumberToObject ( entryJSON, "h",        dest.h             );
			cJSON_AddNumberToObject ( entryJSON, "rotated",  rotated            );

			cJSON_AddItemToArray(rootJSON, entryJSON);
		}
		else
		{
			printf("[ERROR] Couldn't add '%s'\n", images[i].filename);
			fails++;
		}
		SDL_FreeSurface(images[i].surface);
		free(images[i].filename);
	}

	out = cJSON_Print(rootJSON);
	fp  = fopen("atlas.json", "wb");
	fprintf(fp, "%s", out);
	fclose(fp);

	IMG_SavePNG(atlas, "atlas.png");
	free(images);
	SDL_FreeSurface(atlas);
	IDG_FreeNode(root);
	free(root);
	cJSON_Delete(rootJSON);
	free(out);
	printf("Images: %d, Failures: %d, Rotations: %d\n", num_images, fails, rotations);
	SDL_Quit();
}
