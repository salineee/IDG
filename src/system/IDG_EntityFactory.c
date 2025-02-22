#include "../IDG_Common.h"

/* include entities below this line */
#include "../entities/player.h"

#include "IDG_EntityFactory.h"

static init_func_t *IDG_GetInitFunc(char *name);
static void         IDG_AddInitFunc(char *name, void (*init)(entity_t *e));

extern stage_t stage;

static init_func_t head, *tail;

void IDG_InitEntityFactory(void)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Initializing Entity Factory");

    memset(&head, 0, sizeof(init_func_t));
    tail = &head;

    /* add entity init funcs below this line */
    IDG_AddInitFunc("player", init_player);
}

entity_t *IDG_InitEntity(char *name)
{
    entity_t *e;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Creating entity '%s' ...\n", name);
    e = IDG_SpawnEntity();
    IDG_GetInitFunc(name)->init(e);
    
    if(e->type_name != NULL) { STRCPY(e->type_name, name); }
    return e;
}

// Return an array of entities (in use by editor?)
entity_t **IDG_GetEditorEntities(int *total)
{
    init_func_t *i;
    entity_t    *e, **ents;
    int          num = 0;

    // how many entities are we working with..?
    for(i=head.next; i!=NULL; i=i->next)
        num += 1;
    ents = malloc(sizeof(entity_t *)*num);    
    num  = 0;

    for(i=head.next; i!=NULL; i=i->next)
    {
        e = malloc(sizeof(entity_t));
        memset(e, 0, sizeof(entity_t));
        i->init(e);

        STRCPY(e->type_name, i->name);
        ents[num++] = e;
    }

    *total = num;
    return ents;
}

entity_t *IDG_SpawnEntity(void)
{
    entity_t *e;

    e = malloc(sizeof(entity_t));
    memset(e, 0, sizeof(entity_t));

    stage.entity_tail->next = e;
    stage.entity_tail       = e;

    return e;
}

static void IDG_AddInitFunc(char *name, void (*init)(entity_t *e))
{
    init_func_t *i;

    i = malloc(sizeof(init_func_t));
    memset(i, 0, sizeof(init_func_t));
    tail->next = i;
    tail       = i;

    STRCPY(i->name, name);
    i->init = init;
}

static init_func_t *IDG_GetInitFunc(char *name)
{
    init_func_t *i;

    for(i=head.next; i!=NULL; i=i->next)
        if(strcmp(i->name, name) == 0)
            return i;
        
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Unknown entity '%s'\n", name);
    exit(1);
    return NULL;
}