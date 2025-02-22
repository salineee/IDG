- `IDG_*` - Architecture (engine) files

# Engine
### Launch Args
- `(--edit, -E) <map_name>` Launch Editor2D
- `(--map, -M) <map_name>` Launch stage
- `(--debug, -D)` Launch stage with debug options

# Editor2D

# Sprite Atlas Generator
### Launch Args
- `(--size, -S) <px>` Atlas width/height, in pixels 
- `(--dir, -D) <str>` Asset directory to scan
- `(--padding, -P) <px>` Define pixels of padding between atlas cels

## ASAP
- linked list factory

## TODO
- TRIM DOWN LIBPNG DEPENDENCIES!!!!
- Support multiple atlases - would be beneficial to have a system atlas
- Draft engine architecture plots 
- Draft util documentation
- Draft useage documentation
- Modify text drawing to accept variable glyph sizes / Y support
- Pause state (Freeze ticks? prob not. Just stop actions and updates.)
- Make sure aspect ratio and scaling is preserved on fullscreen
- 2D Raycaster
- Macro/script to generate files w/ necessary A_imports and logic/draw functions
- Dynamic name for window header (cmdln args?)

## DOWN THE LINE... 
### Utils
- coyote jump
- freezetime on weapon attacks
- lighting
- physics
- particle effects
- smooth follow camera