#---------------------------------------------------------------------------
# Parallel compilation
MAKEFLAGS += -j$(shell nproc)

# detect OS system

OS_SYSTEM := $(shell uname -s)

ifeq ($(OS),Windows_NT)
    OS_SYSTEM := WINDOWS
endif

#---------------------------------------------------------------------------
# check SDL2 packages on Linux

ifeq ($(OS_SYSTEM), Linux)
    SDL2_CHECK := $(shell pkg-config --exists sdl2 && echo yes || echo no)
    SDL2_IMAGE_CHECK := $(shell pkg-config --exists SDL2_image && echo yes || echo no)
    SDL2_TTF_CHECK := $(shell pkg-config --exists SDL2_ttf && echo yes || echo no)

    ifneq ($(SDL2_CHECK), yes)
        $(warning Warning: SDL2 not found. Please install.)
    endif
    ifneq ($(SDL2_IMAGE_CHECK), yes)
        $(warning Warning: SDL2_image not found. Please install.)
    endif
    ifneq ($(SDL2_TTF_CHECK), yes)
        $(warning Warning: SDL2_ttf not found. Please install.)
    endif
endif

#---------------------------------------------------------------------------
# SDL2 linking options

SDL2_OPTIONS_WINDOWS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
SDL2_OPTIONS_OTHER := -lSDL2 -lSDL2_image -lSDL2_ttf

ifeq ($(OS_SYSTEM), WINDOWS)
    SDL2_OPTIONS := $(SDL2_OPTIONS_WINDOWS)
else
    SDL2_OPTIONS := $(SDL2_OPTIONS_OTHER)
endif

#---------------------------------------------------------------------------
# param for this project

CXX := g++
CXX_FLAGS := -std=c++11

TARGET := game

OFILES := screen.o picture.o map.o character.o coord.o object.o \
            destination.o chest.o score.o

#---------------------------------------------------------------------------
# Emscripten configuration for WebAssembly
EMCC := emcc
EMCC_FLAGS := -std=c++11 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 \
              -s ALLOW_MEMORY_GROWTH=1 -s SDL2_IMAGE_FORMATS='["png", "bmp"]' \
              --preload-file fonts --preload-file imgs --preload-file maps \
              --shell-file resources/shell.html
WASM_TARGET := server/dist/game.html
WASM_OFILES := screen.wasm.o picture.wasm.o map.wasm.o character.wasm.o \
               coord.wasm.o object.wasm.o destination.wasm.o chest.wasm.o score.wasm.o

#---------------------------------------------------------------------------
# Build target for this project
all: $(TARGET)

$(TARGET): ./src/main.cpp $(OFILES)
	g++ $^ -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS) 

screen.o: ./src/views/screen.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

picture.o: ./src/views/picture.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

map.o: ./src/views/map.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

character.o: ./src/object/character.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

coord.o: ./src/util/coord.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

object.o: ./src/object/object.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

destination.o: ./src/object/destination.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

chest.o: ./src/object/chest.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

score.o: ./src/util/score.cpp
	g++ $< -c -o $@ $(CXX_FLAGS) $(SDL2_OPTIONS)

clean:
	rm -f *.o *.wasm.o $(TARGET)
	rm -rf server/dist

#---------------------------------------------------------------------------
# Build WASM object files

screen.wasm.o: ./src/views/screen_wasm.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

picture.wasm.o: ./src/views/picture.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

map.wasm.o: ./src/views/map.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

character.wasm.o: ./src/object/character.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

coord.wasm.o: ./src/util/coord.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

object.wasm.o: ./src/object/object.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

destination.wasm.o: ./src/object/destination.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

chest.wasm.o: ./src/object/chest.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

score.wasm.o: ./src/util/score.cpp
	$(EMCC) $< -c -o $@ $(EMCC_FLAGS)

wasm: $(WASM_TARGET)

$(WASM_TARGET): ./src/main.cpp $(WASM_OFILES)
	mkdir -p server/dist
	$(EMCC) $^ -o $@ $(EMCC_FLAGS)

run_server: wasm
	go run ./server/main.go

build_server: wasm
	CGO_ENABLED=0 go build -o server ./server/main.go

.PHONY: all wasm clean run_server build_server