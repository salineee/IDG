#ifndef __IDG_SOUND__
#define __IDG_SOUND__

void IDG_InitSound      (void);
void IDG_LoadMusic      (char *filename);
void IDG_PlayMusic      (int loop);
void IDG_PlaySound      (int id, int channel);
void IDG_PlaySoundLoop  (int id, int channel);
void IDG_StopChannel    (int channel);

void IDG_SetSoundVolume (int vol);
void IDG_SetMusicVolume (int vol);

#endif // __IDG_SOUND__