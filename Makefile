CC        = gcc
CVER      = -std=c99
# -g    		 // produce debugging info
# -Wall 		 // enables warnings
# -D_REENTRANT   // use declarations necessary for thread usage
CFLAGS    = -g -Wall -D_REENTRANT
LDFLAGS   = -Isrc/Include -Lsrc/lib
SDL_CORE  = -lSDL2main -lSDL2 -lSDL2_image
SDL_MIX   = -lSDL2_mixer
SDL_TTF   = -lSDL2_ttf

CLEAN_PRJ = rm -f *.o

DIR=src

CORE_DIRS=               \
	$(DIR)/*c            \
	$(DIR)/system/*.c    \
	$(DIR)/game/*.c      \
	$(DIR)/entities/*.c
EDITOR_DIRS = $(DIR)/editor/*.c
ATLAS_DIRS  = $(DIR)/atlas_gen/*.c 
JSON_DIRS   = $(DIR)/json/*.c

CORE_OBJS=               \
	main.o               \
	IDG_ai.o             \
	IDG_Atlas.o          \
	IDG_Brush.o          \
	IDG_Camera.o         \
	IDG_Draw.o           \
	IDG_Editor_Map2D.o   \
	IDG_Editor2D.o       \
	IDG_Effect.o         \
	IDG_Entities.o       \
	IDG_EntityFactory.o  \
	IDG_Game.o           \
	IDG_Init.o           \
	IDG_Input.o          \
	IDG_Map2D.o          \
	IDG_Options.o        \
	IDG_Quadtree.o       \
	IDG_Sound.o          \
	IDG_Text.o           \
	IDG_Textures.o       \
	IDG_UI2D.o           \
	IDG_Util.o           \
	IDG_Widgets.o        \
	cJSON.o              \
	hud.o                \
	player.o             \
	stage.o

ATLAS_OBJS=              \
	IDG_AtlasGen.o       \
	cJSON.o

# TODO
# Compile this to a folder

#.exe
all:
	$(CC) $(CFLAGS) $(CVER) -o main              \
	$(CORE_DIRS) $(EDITOR_DIRS) $(JSON_DIRS)     \
	$(LDFLAGS) $(SDL_CORE) $(SDL_MIX) $(SDL_TTF)

# .a
all_a:
	$(CC) $(CFLAGS) $(CVER) -c                   \
	$(CORE_DIRS) $(JSON_DIRS) $(EDITOR_DIRS)     \
	$(LDFLAGS) $(SDL_CORE) $(SDL_MIX) $(SDL_TTF) \
	ar rcs idg_engine.a $(CORE_OBJS)
	$(CLEAN_PRJ)
atlas: 
	$(CC) $(CLFAGS) $(CVER) -c           		 \
	$(ATLAS_DIRS) $(JSON_DIRS)           		 \
	$(LDFLAGS) $(SDL_CORE)

	$(CC) $(CFLAGS) $(CVER) -o atlas_gen 		 \
	$(ATLAS_OBJS)                                \
	$(LDFLAGS) $(SDL_CORE)
	$(CLEAN_PRJ)
clean:
	$(CLEAN_PRJ) *.exe idg_engine.a





all_old:
	gcc -Wall -std=c99 -o main src/*.c src/json/*.c src/system/*.c src/game/*.c src/editor/*.c src/entities/*.c -D_REENTRANT -Isrc/Include -Lsrc/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

all_old_old:
	gcc -Wall -std=c99 -o main src/*.c src/json/*.c src/system/*.c src/game/*.c src/entities/*.c -D_REENTRANT -Isrc/Include -Lsrc/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

atlas_old:
	gcc -Wall -c src/atlas_gen/*.c src/json/*.c -Isrc/Include -Lsrc/lib -lSDL2main -lSDL2 -lSDL2_image
	gcc -Wall -o atlas_gen IDG_AtlasGen.o cJSON.o -Isrc/Include -Lsrc/lib -lSDL2main -lSDL2 -lSDL2_image