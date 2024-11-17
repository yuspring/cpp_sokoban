#---------------------------------------------------------------------------
#detect OS system

OS_SYSTEM := $(shell uname -s)

ifeq ($(OS),Windows_NT)
	OS_SYSTEM := WINDOWS
endif

SDL2_OPTIONS_WINDOWS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
SDL2_OPTIONS_OTHER := -lSDL2 -lSDL2_image -lSDL2_ttf

ifeq ($(OS_SYSTEM), WINDOWS)
	SDL2_OPTIONS := $(SDL2_OPTIONS_WINDOWS)
else
	SDL2_OPTIONS := $(SDL2_OPTIONS_OTHER)
endif

#---------------------------------------------------------------------------
#param for this project

CXX := g++
CXX_FLAGS := -std=c++11

TARGET := game

OFILES := screen.o picture.o map.o character.o coord.o object.o \
			destination.o chest.o score.o

#---------------------------------------------------------------------------
#build target for this project
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
	rm -f *.o $(TARGET)

.PHONY: all clean