#include "../IDG_Common.h"

#include "../system/IDG_Util.h"

#include "IDG_Quadtree.h"

#define MAX_DEPTH           3
#define QT_INITIAL_CAPACITY 8

extern stage_t stage;

static int  IDG_GetIndex             (quadtree_t *root, int x, int y, int w, int h);
static void IDG_RemoveEntity         (entity_t *e, quadtree_t *root);
static int  IDG_EntityComparator     (const void *a, const void *b);
static void IDG_GetAllEntsWithinNode (int x, int y, int w, int h, entity_t **candidates, entity_t *ignore, quadtree_t *root);
static void IDG_DestroyQuadtreeNode  (quadtree_t *root);
static void IDG_ResizeQTEntCapacity  (quadtree_t *root);

static int c_idx;
static int num_cells;

void IDG_InitQuadtree(quadtree_t *root)
{
    quadtree_t *node;
    int i, w, h;

    // entire map - top level of the tree
    if(root->depth == 0)
    {
        root->w        = (MAP_WIDTH*MAP_TILE_SIZE);
        root->h        = (MAP_HEIGHT*MAP_TILE_SIZE);
        root->capacity = QT_INITIAL_CAPACITY;
        root->ents     = malloc(sizeof(entity_t *)*root->capacity);
        memset(root->ents, 0, sizeof(entity_t *)*root->capacity);
        num_cells      = 1; // just for debug output
    }

    if(root->depth < MAX_DEPTH)
    {
        // split the current cell in half
        w = root->w/2;
        h = root->h/2;

        // setup quadrants of the new cell
        for(i=0; i<4; i++)
        {
            node = malloc(sizeof(quadtree_t));
            memset(node, 0, sizeof(quadtree_t));
            root->node[i] = node;

            node->depth = (root->depth+1); // this cell is the child of the current quadtree - therefor is one level deeper
            node->capacity = QT_INITIAL_CAPACITY;
            node->ents = malloc(sizeof(entity_t *)*node->capacity);
            memset(node->ents, 0, sizeof(entity_t *)*node->capacity);

            // set this node's w,h to the halved dimensions of the parent node
            node->w = w;
            node->h = h;

            switch(i)
            {
                case 0: // top left quarter
                    node->x = root->x;
                    node->y = root->y;
                    break;
                case 1: // top right quarter
                    node->x = (root->x+w);
                    node->y = root->y;
                    break;
                case 2: // bottom left quarter
                    node->x = root->x;
                    node->y = (root->y+h);
                    break;
                default: // bottom right quarter
                    node->x = (root->x+w);
                    node->y = (root->y+h);
                    break;
            }

            num_cells++;            // just for debug output
            IDG_InitQuadtree(node); // recurse until we reach max depth
        }
    }

    if(root->depth == 0)
    {
        SDL_LogMessage(
            SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, 
            "Quadtree: [num_cells = %d, memory = %.2fkb]", 
            num_cells, (double)((sizeof(quadtree_t)*num_cells)/1024.0)
        );
    }
}

void IDG_AddToQuadtree(entity_t *e, quadtree_t *root)
{
    if(!e->texture)
    {
        SDL_LogMessage(
            SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
            "*** Entity '%s' has no texture. Unable to add to quadtree. ***",
            e->type_name
        );
    }

    int idx;

    root->added_to = 1; // an entity has been added

    // does the current cell have any child nodes?
    if(root->node[0] != NULL)
    {
        // which quadrant the entity is within
        // Current entity (player) doesn't have a texture - so no w, h
        idx = IDG_GetIndex(root, e->x, e->y, e->texture->rect.w, e->texture->rect.h);

        // if entity does not exceed max node entities,
        // call recursively and add entity to the next child node
        if(idx != -1)
        {
            IDG_AddToQuadtree(e, root->node[idx]);
            return;
        }
    }
    // if index is -1 or we're at the bottom of the tree,
    // add entity to the quadrant.

    // first check if the node's entity capacity is full, 
    // resize the node if so. 
    if(root->num_ents == root->capacity)
        IDG_ResizeQTEntCapacity(root);
    
    root->ents[root->num_ents++] = e;
}

static void IDG_ResizeQTEntCapacity(quadtree_t *root)
{
    int n;

    n = (root->capacity+QT_INITIAL_CAPACITY);
    
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Resizing QT node: %d -> %d", root->capacity, n);
    
    root->ents = IDG_ResizeArray(root->ents, sizeof(entity_t *)*root->capacity, sizeof(entity_t *)*n);
    root->capacity = n;
}

static int IDG_GetIndex(quadtree_t *root, int x, int y, int w, int h)
{
    int v_mid, h_mid, // vertical middlepoint, horizontal middlepoint
        top_h, bot_h; // top half, bottom half
    
    // Current entity (player) doesn't have a texture - so no w, h
    v_mid = root->x+(root->w/2);          // node's x location + half its width
    h_mid = root->y+(root->h/2);          // node's y location + half its height
    top_h = (y < h_mid && (y+h) < h_mid); // boolean - entity fits in top half of node? 
    bot_h = (y > h_mid);                  // boolean - entity fits in top half of node?

    if(x < v_mid && (x+w) < v_mid) // does the entity fit in the left side of the quadrant?
    {
        if(top_h) { return 0; }    // top left
        if(bot_h) { return 2; }    // bottom left
    }
    else if(x > v_mid)            // does the entity fit in the right side of the quadrant?
    {
        if(top_h) { return 1; }   // top half
        if(bot_h) { return 3; }   // bottom half
    }
    return -1; // the entity is contained within the current node, but will not fit in any child node
}

/*** TODO **************************************************************
 * this function recursively searches for an entity within the quadtree, 
 * in the same manner in which it was added.
 * 
 * this can be optimized by storing the node the entity was
 * added to, for direct access. and removal 
************************************************************************/
void IDG_RemoveFromQuadtree(entity_t *e, quadtree_t *root)
{
    int idx;
    if(root->added_to)
    {
        if(root->node[0] != NULL)
        {
            idx = IDG_GetIndex(root, e->x, e->y, e->texture->rect.w, e->texture->rect.h);
            if(idx != -1)
            {
                IDG_RemoveFromQuadtree(e, root->node[idx]);
                return;
            }
        }

        IDG_RemoveEntity(e, root);

        // test how many entities remain in the node
        if(root->num_ents == 0)
        {
            root->added_to = 0;
            if(root->node[0] != NULL)
                root->added_to = root->node[0]->added_to || root->node[1]->added_to || root->node[2]->added_to || root->node[3]->added_to;
        }
    }
}

static void IDG_RemoveEntity(entity_t *e, quadtree_t *root)
{
    int i, n;
    n = root->num_ents; // how many entities existed before removal?

    for(i=0; i<n; i++)
    {
        if(root->ents[i] == e)
        {
            root->ents[i] = NULL;
            root->num_ents--;
        }
    }

    // shift all non-NULL entities to the front of the array,
    // so adding new entities can be done at NULL entries
    qsort(root->ents, n, sizeof(entity_t *), IDG_EntityComparator);
}

void IDG_GetAllEntsWithin(int x, int y, int w, int h, entity_t **candidates, entity_t *ignore)
{
    c_idx = 0;
    memset(candidates, 0, sizeof(entity_t *)*MAX_QT_CANDIDATES);
    IDG_GetAllEntsWithinNode(x, y, w, h, candidates, ignore, &stage.quadtree);
}

static void IDG_GetAllEntsWithinNode(int x, int y, int w, int h, entity_t **candidates, entity_t *ignore, quadtree_t *root)
{
    int idx, i;

    // don't bother checking for entities if the node is flagged empty
    if(root->added_to)
    {
        if(root->node[0] != NULL)
        {
            idx = IDG_GetIndex(root, x, y, w, h);
            if(idx != -1)
                IDG_GetAllEntsWithinNode(x, y, w, h, candidates, ignore, root->node[idx]);
            else
                for(i=0; i<4; i++)
                    IDG_GetAllEntsWithinNode(x, y, w, h, candidates, ignore, root->node[i]);
        }

        for(i=0; i<root->num_ents; i++)
        {
            if(c_idx < MAX_QT_CANDIDATES)
            {
                if(root->ents[i] != ignore)
                {
                    candidates[c_idx++] = root->ents[i];
                }
            }
            else
            {
                SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Out of quadtree candidate space (%d)", MAX_QT_CANDIDATES);
                exit(1);
            }
        }
    }
}

void IDG_DestroyQuadtree(quadtree_t *root)
{
    IDG_DestroyQuadtreeNode(root);
}

static void IDG_DestroyQuadtreeNode(quadtree_t *root)
{
    int i;

    free(root->ents);
    root->ents = NULL;

    if(root->node[0] != NULL)
    {
        for(i=0; i<4; i++)
        {
            IDG_DestroyQuadtreeNode(root->node[i]);
            free(root->node[i]);
            root->node[i] = NULL;
        }
    }
}

// TODO - Move this to utils or something.
static int IDG_EntityComparator(const void *a, const void *b)
{
    entity_t *e1 = *((entity_t **)a);
    entity_t *e2 = *((entity_t **)b);

    if(e1 == NULL)      { return 1;  }
    else if(e2 == NULL) { return -1; }
    else                { return 0;  }
}