# Compiler and flags
CC = g++
CFLAGS = -Wall -g -std=c++11 -Iinclude

# Source files
SRCS = main.cpp BacktrackData.cpp Connection.cpp LevelData.cpp MapData.cpp Room.cpp

# Object files
OBJ_DIR = ./build
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Executable name
TARGET = rogueSysMapExporter

# Default target
all: $(TARGET)

# Create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Linking step
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compilation step
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)