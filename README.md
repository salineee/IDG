# IDGv0.1

- `IDG_*` - Architecture (engine) files.<br>
Game-specific code will sometimes be placed in `IDG_*` files - <br>
a good example is `IDG_EntityFactory.c`. Game-specific entities must be included here to be initialized.

# Engine
Compile the core engine using `make`.<br>
Launch using `main --edit <map_name>`.<br>
Launching `IDG` with a <map_name> expects three files present in `data/stages` -
- `<map_name>.map` Atlas of tile indexes to be rendered  
- `<map_name>.json` Entities to be processed for the stage
- `<map_name>_sectors.json` Sectors to be processed for the stage 
### Launch Args
- `(--edit, -E) <map_name>` Launch Editor2D
- `(--map, -M) <map_name>` Launch stage
- `(--debug, -D)` Launch stage in debug mode

# Editor2D
Compile `Editor2D` using make`.

- `1` - Tile Mode
- `2` - Sector Mode
- `3` - Entity Mode (currently broken)
- `4` - Pick Mode (currently broken)
- `8` - Toggle Show Sectors 
- `9` - Toggle Show Grid
- `0` - Toggle Show Minimap
- `[` - Decrease Brush size in Tile Mode
- `]` - Increase Brush size in Tile Mode
- `Space` - Save Map

# Sprite Atlas Generator
### Launch Args
- `(--size, -S) <px>` Atlas width/height, in pixels 
- `(--dir, -D) <str>` Asset directory to scan
- `(--padding, -P) <px>` Define pixels of padding between atlas cels

## TODO
### <i>... ever changing. never complete, never concise, never definitive.</i>
- Compile as a library for easier use in other projects
- Draft util documentation
- Draft useage documentation
- Linux support
- Either enforce 64 bit, or support 32 bit.
- TRIM DOWN LIBPNG DEPENDENCIES!!!!
- BETTER LOGGING!!!
- SET UP GDB!!!
- Isolate physics functions
- Support multiple stage cameras
- Support smooth camera movement
- Preserve camera pos offsets on scaling
- Canvas modulation
- Support multiple sprite atlases
- Modify text renderer to accept variable glyph sizes / Y support
- Fix window sizing and scaling
- 2D Raycaster
- Macro/script to generate files w/ necessary `IDG_*.h` imports and logic/draw functions
- Dynamic name for window header (cmdln args?)
- Fix out of bounds (overflow?) corruption error in editor/tile mode