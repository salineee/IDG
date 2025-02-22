#ifndef __IDG_EDITOR2D__
#define __IDG_EDITOR2D__

#define BOTTOM_HUD_PADDING 2
#define ENTITY_SPACING     8
#define SCROLL_OVERSCAN   (MAP_TILE_SIZE*8)
#define MOVE_MODE_SPEED    3
#define MINIMAP_CEL_SIZE   3

enum
{
	MODE_TILES,	   // place/remove tiles
	MODE_SECTOR,   // create lines, sectors
	MODE_ENTITIES, // place/remove entities
	MODE_PICK 	   // reposition existing entities
};

void IDG_InitEditor2D(void);

#endif // __IDG_EDITOR2D__