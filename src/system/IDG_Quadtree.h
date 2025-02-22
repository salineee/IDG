#ifndef __IDG_QUADTREE__
#define __IDG_QUADTREE__

void IDG_InitQuadtree       (quadtree_t *root);
void IDG_AddToQuadtree      (entity_t *e, quadtree_t *root);
void IDG_RemoveFromQuadtree (entity_t *e, quadtree_t *root);
void IDG_GetAllEntsWithin   (int x, int y, int w, int h, entity_t **candidates, entity_t *ignore);
void IDG_DestroyQuadtree    (quadtree_t *root);

#endif // __IDG_QUADTREE__