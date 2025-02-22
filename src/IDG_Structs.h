#ifndef __IDG_STRUCTS__
#define __IDG_STRUCTS__

typedef struct AtlasImage atlas_image_t;
typedef struct Effect     effect_t;
typedef struct Entity     entity_t;
typedef struct InitFunc   init_func_t;
typedef struct Node       Node;          // CURRENTLY HAS ATTRS FOR ATLASGEN
typedef struct Quadtree   quadtree_t;
typedef struct Sector2D   sector_2D_t;
typedef struct Texture    texture_t;
typedef struct Widget     widget_t;

// TODO- why does this throw errors when named node_t?
struct Node
{
    int          x;
    int          y;
    int          w;
    int          h;
    int          used;
    struct Node *child;
};

typedef struct
{
    SDL_Surface *surface;
    char        *filename;
} image_t;

struct AtlasImage
{
    SDL_Texture   *texture;
    SDL_Rect       rect;
    char           filename[MAX_FILENAME_LENGTH];
    int            rotated;
    atlas_image_t *next;
};

struct Texture
{
    SDL_Texture *texture;
    char         name[MAX_FILENAME_LENGTH];
    texture_t   *next;
};

struct InitFunc
{
    char         name[MAX_NAME_LENGTH];
    void       (*init)(entity_t *e);
    init_func_t *next;
};

struct Quadtree
{
    int         x, y, w, h; // coords and dimensions of the cel
    int         depth;      // depth at which the cel resides
    entity_t  **ents;       // array of entities that reside in current cel
    int         capacity;   // size of ents array
    int         num_ents;   // how many entities currently occupy the cel 
    int         added_to;   // whether the cel and its children have had entities added to it 
    quadtree_t *node[4];
};

struct Sector2D
{
    int           sector_id;
    int           x;
    int           y;
    int           w;
    int           h;
    int           layer;
    int           material;
    int           interactive;
    int           solid;
    int           blocks_los;
    unsigned long flags;
    sector_2D_t  *next;
};

// TODO - This is not project-specific. Just general entity stuff.
struct Entity
{
    char           name[MAX_NAME_LENGTH];
    char           type_name[MAX_NAME_LENGTH];
    double         x;            // x position
    double         y;            // y position
    double         z;            // z index
    double         dx;           // delta x
    double         dy;           // delta y
    double         friction;
    int            w;            // width
    int            h;            // height
    int            dead;         // should be moved to free queue?
    int            facing;       // facing direction? 2D relevant only
    int            is_on_ground; 
    int            layer;
    int            angle;
    unsigned long  flags;
    unsigned long  editor_flags;
    atlas_image_t *texture;
    entity_t      *riding;       // entity is on a moving object / entity?
    SDL_Rect       hitbox;
    void         (*data)        (entity_t *self);
    void         (*tick)        (entity_t *self);
    void         (*draw)        (entity_t *self);
    void         (*touch)       (entity_t *self, entity_t *other);
    void         (*die)         (entity_t *self);
    void         (*take_damage) (entity_t *self, int amount, entity_t *attacker);
    void         (*post_load)   (entity_t *self);
    entity_t      *next;
};

/* WIDGET STRUCTS */

struct Widget // Core widget
{
    char      name[MAX_NAME_LENGTH];
    char      label[MAX_NAME_LENGTH];
    char      group_name[MAX_NAME_LENGTH];
    int       type;
    int       x;
    int       y;
    int       w;
    int       h;
    void    (*action) (void);
    void    (*data);
    widget_t *prev;
    widget_t *next;
};

typedef struct // Select 
{
    char **options;     // Specified in JSON
    int    num_options;
    int    x;
    int    y;
    int    value;
} select_widget_t;

typedef struct // Slider
{
    int x;
    int y;
    int w;
    int h;
    int value;
    int step;
    int wait_on_change;
} slider_widget_t;

typedef struct // Text input
{
    int   x;
    int   y;
    int   max_length;
    char *text;
} input_widget_t;

typedef struct // Control
{
    int x;
    int y;
    int value;
    int keyboard;
    int joypad;
} control_widget_t;

/* END WIDGET STRUCTS */

struct Effect
{
    double    x;
    double    y;
    double    dx;
    double    dy;
    double    life;
    double    alpha;
    int       size;
    SDL_Color color;
    effect_t *next;
};

typedef struct
{
    int keyboard_controls[CTRL_MAX];
    int joypad_controls[CTRL_MAX];
    int deadzone;
    int sound_volume;
    int music_volume;
} game_t;

typedef struct
{
    struct
    {
        SDL_Point pos;
        int       initialized;
    } camera;

    char        name[MAX_NAME_LENGTH];
    int         map[MAP_WIDTH][MAP_HEIGHT];
    int         status; // level complete/failed/etc
    double      time;   // time elapsed in level
    entity_t    entity_head,      *entity_tail;
    entity_t    dead_entity_head, *dead_entity_tail;
    effect_t    effect_head,      *effect_tail;
    sector_2D_t sector2D_head,    *sector2D_tail;   
    entity_t   *player;
    quadtree_t  quadtree;
} stage_t;

typedef struct
{
    struct
    {
        void (*logic) (void);
        void (*draw)  (void);
    } delegate;
    
    struct
    {
        int x;
        int y;
        int buttons[MAX_MOUSE_EVENTS];
        int visible;
    } mouse;

    struct
    {
        int fps;

        // launch flags
        int is_editor;
        int is_debug;

        // editor fields
        int curr_sector_id;
        int show_grid;
        int show_minimap;
        int show_sectors;
        int sector_menu_focused; // is viewing sector metadata menu
        int snap_to_grind;       // currently used for sectors only - UNUSED CURRENTLY

        // stage devtools
        int show_fps;
        int show_debug;
        int show_hitboxes;
        int collision_checks;
    } dev;

    SDL_Renderer *renderer;
    SDL_Window   *window;

    SDL_Joystick *joypad;
    int           joypad_button[MAX_JOYPAD_BUTTONS];
    int           joypad_axis[JAXIS_MAX];
    int           keyboard[MAX_KEYBOARD_EVENTS];
    int           last_key_pressed;
    int           last_button_pressed;
    
    double        delta_time;
    double        texture_scale;
    double        font_scale;

    char          input_text[MAX_INPUT_LENGTH];
    widget_t     *active_widget;
} app_t;

#endif // __IDG_STRUCTS__