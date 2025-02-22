#include <SDL2/SDL_ttf.h>

#include "../IDG_Common.h"

#include "../system/IDG_Textures.h"

#include "IDG_Text.h"

#define FONT_SIZE          48
#define FONT_TEXTURE_SIZE  512
#define NUM_GLYPHS         128
#define MAX_WORD_LENGTH    32
#define MAX_LINE_LENGTH    1024

extern app_t app;

static void IDG_InitFont        (char *filename);
static int  IDG_DrawTextWrapped (char *text, int x, int y, int r, int g, int b, int align, int max_width, int draw);
static void IDG_DrawTextLine    (char *text, int x, int y, int r, int g, int b, int align);

static SDL_Color    white = {255, 255, 255, 255};
static SDL_Rect     glyphs[NUM_GLYPHS];
static SDL_Texture *font_texture;
static TTF_Font    *font;

void IDG_InitFonts(void)
{
    // Refactor this - should read from fonts directory
    IDG_InitFont("fonts/EnterCommand.ttf");
	app.font_scale = 1;
}

static void IDG_InitFont(char *filename)
{
    SDL_Surface *surface, *text;
	SDL_Rect     dest;
	int          i;
	char         c[2];
	SDL_Rect    *g;

	memset(&glyphs, 0, sizeof(SDL_Rect) * NUM_GLYPHS);

	font    = TTF_OpenFont(filename, FONT_SIZE);
	surface = SDL_CreateRGBSurface(0, FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE, 32, 0, 0, 0, 0xff);
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
	dest.x = dest.y = 0;

	for (i = ' '; i <= 'z'; i++)
	{
		c[0] = i;
		c[1] = 0;

		text = TTF_RenderUTF8_Blended(font, c, white);
		TTF_SizeText(font, c, &dest.w, &dest.h);

		if (dest.x + dest.w >= FONT_TEXTURE_SIZE)
		{
			dest.x = 0;
			dest.y += dest.h + 1;

			if (dest.y + dest.h >= FONT_TEXTURE_SIZE)
			{
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Out of glyph space in %dx%d font atlas texture map.", FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE);
				exit(1);
			}
		}

		SDL_BlitSurface(text, NULL, surface, &dest);

		g    = &glyphs[i];
		g->x = dest.x;
		g->y = dest.y;
		g->w = dest.w;
		g->h = dest.h;

		SDL_FreeSurface(text);
		dest.x += dest.w;
	}
	font_texture = IDG_ToTexture(surface, 1);
}

SDL_Texture *IDG_GetTextTexture(char *text, int type)
{
    SDL_Surface *surface;
	surface = TTF_RenderUTF8_Blended(font, text, white);
	return IDG_ToTexture(surface, 1);
}

void IDG_DrawText(char *text, int x, int y, int r, int g, int b, int align, int max_width)
{
	if (max_width > 0)
		IDG_DrawTextWrapped(text, x, y, r, g, b, align, max_width, 1);
	else
		IDG_DrawTextLine(text, x, y, r, g, b, align);
}

static int IDG_DrawTextWrapped(char *text, int x, int y, int r, int g, int b, int align, int max_width, int do_draw)
{
    char word[MAX_WORD_LENGTH], line[MAX_LINE_LENGTH];
	int  i, n, word_width, line_width, character, len, new_line, clear_word;
	i = 0;

	memset(word, 0, MAX_WORD_LENGTH);
	memset(line, 0, MAX_LINE_LENGTH);

	character = text[i++];
	n         = new_line = clear_word = line_width = word_width = 0;
	len       = strlen(text);

	while (character)
	{
		if (!new_line)
		{
			word_width += glyphs[character].w * app.font_scale;
			if (character != ' ')
				word[n++] = character;
		}

		if (character == ' ' || i == len || new_line)
		{
			if (line_width + word_width >= max_width || new_line)
			{
				if (do_draw)
					IDG_DrawTextLine(line, x, y, r, g, b, align);

				memset(line, 0, MAX_LINE_LENGTH);

				y += glyphs[' '].h * app.font_scale;
				line_width = 0;
				new_line = 0;
			}
			clear_word = 1;
		}

		character = text[i++];
		if (character == '\n')
		{
			new_line = 1;
			clear_word = 1;
		}

		if (clear_word)
		{
			clear_word = 0;
			if (line_width != 0)
				strcat(line, " ");
			strcat(line, word);
			line_width += word_width;
			memset(word, 0, MAX_WORD_LENGTH);
			word_width = 0;
			n = 0;
		}
	}

	if (do_draw)
		IDG_DrawTextLine(line, x, y, r, g, b, align);
	return y + glyphs[' '].h * app.font_scale;
}

static void IDG_DrawTextLine(char *text, int x, int y, int r, int g, int b, int align)
{
    int       i, character, w, h;
	SDL_Rect *glyph, dest;

	if (align != TEXT_ALIGN_LEFT)
	{
		IDG_GetTextDimensions(text, &w, &h);
		if (align == TEXT_ALIGN_CENTER)
			x -= (w / 2);
		else if (align == TEXT_ALIGN_RIGHT)
			x -= w;
	}

	SDL_SetTextureColorMod(font_texture, r, g, b);

	i         = 0;
	character = text[i++];
	while (character)
	{
		glyph  = &glyphs[character];

		dest.x = x;
		dest.y = y;
		dest.w = glyph->w * app.font_scale;
		dest.h = glyph->h * app.font_scale;

		SDL_RenderCopy(app.renderer, font_texture, glyph, &dest);
		x += glyph->w * app.font_scale;
		character = text[i++];
	}
}

void IDG_GetTextDimensions(char *text, int *w, int *h)
{
    int       i, character;
	SDL_Rect *g;

	*w = *h = 0;
	i  = 0;
	character = text[i++];
	while (character)
	{
		g         = &glyphs[character];
		*w       += g->w * app.font_scale;
		*h        = MAX(g->h * app.font_scale, *h);
		character = text[i++];
	}
}

int IDG_GetWrappedTextHeight(char *text, int max_width)
{
    return IDG_DrawTextWrapped(text, 0, 0, 255, 255, 255, TEXT_ALIGN_LEFT, max_width, 0);
}