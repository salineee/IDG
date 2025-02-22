#include <SDL2/SDL_mixer.h>

#include "../IDG_Common.h"

#include "IDG_Sound.h"

static void IDG_LoadSound(void);

static Mix_Chunk *sounds[SND_MAX];
static Mix_Music *music;

/*** TODO *****************************
 * This file should be modified to take
 * a dynamic list of sounds to load
 * on IDG_InitSound exec
**************************************/

void IDG_InitSound(void)
{
    memset(sounds, 0, sizeof(Mix_Chunk *)*SND_MAX);
    music = NULL;
    IDG_LoadSound();
}

void IDG_LoadMusic(char *filename)
{
    if(music != NULL)
    {
        Mix_HaltMusic ();
        Mix_FreeMusic (music);
        music = NULL;
    }
    music = Mix_LoadMUS(filename);
}

void IDG_SetSoundVolume(int vol)
{
    Mix_Volume(-1, vol);
}

void IDG_SetMusicVolume(int vol)
{
    Mix_VolumeMusic(vol);
}

void IDG_PlayMusic(int loop)
{
    Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void IDG_PlaySound(int id, int channel)
{
    Mix_PlayChannel(channel, sounds[id], 0);
}

void IDG_StopChannel(int channel)
{
    Mix_HaltChannel(channel);
}

static void IDG_LoadSound(void)
{
    /* load in sounds here */
    // sounds[SND_ID] = Mix_LoadWAV(sound_path);
}