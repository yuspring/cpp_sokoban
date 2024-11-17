# Detect OS and architecture
OS_SYSTEM := $(shell uname -s)
ARCH := $(shell uname -m)

ifeq ($(OS),Windows_NT)
	OS_SYSTEM := WINDOWS
endif

ifeq ($(OS_SYSTEM),Darwin)
	ifeq ($(ARCH),arm64)
		OS_SYSTEM := MACARM
	else
		OS_SYSTEM := MAC64
	endif
endif

# SDL2 configuration
#SDL2_LIB_WINDOWS := -Ithird_party/include -Lthird_party/lib
SDL2_LIB_MACARM := -I/opt/homebrew/include -L/opt/homebrew/lib
SDL2_LIB := $(SDL2_LIB_$(OS_SYSTEM))

SDL2_OPTIONS_WINDOWS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
SDL2_OPTIONS_OTHER := -lSDL2 -lSDL2_image -lSDL2_ttf
SDL2_OPTIONS := $(SDL2_OPTIONS_$(if $(filter WINDOWS,$(OS_SYSTEM)),WINDOWS,OTHER))

# Compiler and flags
CXX := g++
CXX_FLAGS := -std=c++11

# Source files and directories
SRC_DIR := ./src
OBJ_DIR := ./build
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Target executable
TARGET := game

# Default target
all: $(TARGET)

# Build rules
$(TARGET): $(OBJ_FILES)
	$(CXX) $^ -o $@ $(SDL2_LIB) $(SDL2_OPTIONS) $(CXX_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $< -c -o $@ $(SDL2_LIB) $(SDL2_OPTIONS) $(CXX_FLAGS)

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
