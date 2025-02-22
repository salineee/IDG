#include <SDL2/SDL_mixer.h>

#include "../IDG_Common.h"

#include "../system/IDG_Sound.h"
#include "../system/IDG_Util.h"
#include "../json/cJSON.h"

#include "IDG_Game.h"

#define SAVE_GAME_FILENAME "save.json"

extern game_t game;

static void IDG_LoadGame(void);

void IDG_InitGame(void)
{
    game.sound_volume = MIX_MAX_VOLUME;
    game.music_volume = MIX_MAX_VOLUME;

    /* include controls definitions below this line */
    /*******/

    IDG_LoadGame        ();
    IDG_SetSoundVolume (game.sound_volume);
    IDG_SetMusicVolume (game.music_volume);
}

static void IDG_LoadGame(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading Game from '%s'", SAVE_GAME_FILENAME);

    cJSON *root, *controls, *node;
    char  *data;

    data = IDG_ReadFile(SAVE_GAME_FILENAME);

    if(data != NULL)
    {
        root     = cJSON_Parse         (data);
        controls = cJSON_GetObjectItem (root, "controls");

        node = cJSON_GetObjectItem (controls, "keyboard");
        /* include keyboard controls below this line */
        // game.keyboard_controls[CTRL_ID] = cJSON_GetObjectItem ( node, "ctrl")->valueint;

        node = cJSON_GetObjectItem (controls, "joypad");
        /* include joypad controls below this line */
        // game.joypad_controls[CTRL_ID] = cJSON_GetObjectItem ( node, "ctrl")->valueint;

        node = cJSON_GetObjectItem (root, "volumes");
        game.sound_volume = MAX(MIN(cJSON_GetObjectItem(node, "sound")->valueint, MIX_MAX_VOLUME), 0);
        game.music_volume = MAX(MIN(cJSON_GetObjectItem(node, "music")->valueint, MIX_MAX_VOLUME), 0);
    }
}

void IDG_SaveGame(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Saving Game into '%s'", SAVE_GAME_FILENAME);

    cJSON *root, *controls, *keyboard, *joypad, *volumes;
    char  *out;

    keyboard = cJSON_CreateObject();
    /* include keyboard controls below this line */
    // cJSON_AddNumberToObject(keyboard, "ctrl", game.keyboard_controls[CTRL_ID]);

    joypad   = cJSON_CreateObject();
    /* include joypad controls below this line */
    // cJSON_AddNumberToObject(joypad, "ctrl", game.joypad_controls[CTRL_ID]);

    volumes  = cJSON_CreateObject();
    cJSON_AddNumberToObject ( volumes, "sound", game.sound_volume );
    cJSON_AddNumberToObject ( volumes, "music", game.music_volume );

    controls = cJSON_CreateObject();
    cJSON_AddItemToObject   ( controls, "keyboard", keyboard );
    cJSON_AddItemToObject   ( controls, "joypad",   joypad   );

    root     = cJSON_CreateObject();
    cJSON_AddItemToObject   ( root, "controls", controls );
    cJSON_AddItemToObject   ( root, "volumes",  volumes  );

    out = cJSON_Print(root);
    IDG_WriteFile(SAVE_GAME_FILENAME, out);

    cJSON_Delete(root);
    free(out);
}