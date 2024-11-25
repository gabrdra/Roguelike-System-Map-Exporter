# Compiler and flags
CC = g++
CFLAGS = -Wall -g -std=c++11 -Iinclude

# Source files
SRCS = src/main.cpp src/BacktrackData.cpp src/Connection.cpp src/LevelData.cpp src/MapData.cpp src/Room.cpp

# Object files
OBJ_DIR = ./build
OBJS = $(SRCS:src/%.cpp=$(OBJ_DIR)/%.o)

# Executable name
TARGET = my_program

# Default target
all: $(TARGET)

# Create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Linking step
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compilation step
$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)