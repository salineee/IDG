#ifndef __IDG_ENTITYFACTORY__
#define __IDG_ENTITYFACTORY__

void       IDG_InitEntityFactory (void);
entity_t  *IDG_InitEntity        (char *name);
entity_t **IDG_GetEditorEntities (int *total);
entity_t  *IDG_SpawnEntity       (void);

#endif // __IDG_ENTITYFACTORY__