# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -DIS_RP3D_DOUBLE_PRECISION_ENABLED

# Libraries
LIBS := -lglfw -lGLEW -lGL -lm -ldl -lpthread -lassimp -lreactphysics3d

# Directories
SRC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Files
TARGET = $(BIN_DIR)/main
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard *.cpp) $(wildcard $(SRC_DIR)/imgui/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(TARGET)

# Build target executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) -g

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/imgui
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the project
run: all
	./$(TARGET)

remake: clean run

# Phony targets
.PHONY: all clean run
