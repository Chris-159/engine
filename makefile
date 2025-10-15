# Compiler
CXX := g++
CXXFlags := -std=c++17 -Wall -Wextra -Icore -Iloader

# Dirs
SRC_DIRS := core loader math transform
OBJ_DIR := obj
BIN := engine

# Source files
SRC_FILES := main.cpp $(wildcard loader/*.cpp) $(wildcard core/*.cpp)

# Object files
OBJ_FILES := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC_FILES)))

all: $(BIN)

# Link
$(BIN): $(OBJ_FILES)
	@echo "🔧 Linking $@..."
	$(CXX) $(OBJ_FILES) -o $(BIN)
	@echo "✅ Build complete."

# Compile main.cpp (root)
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "🧱 Compiling root source: $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile loader .cpp -> .o
$(OBJ_DIR)/%.o: loader/%.cpp | $(OBJ_DIR)
	@echo "🧱 Compiling loader $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile core .cpp -> .o
$(OBJ_DIR)/%.o: core/%.cpp | $(OBJ_DIR)
	@echo "🧱 Compiling core $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure obj dir exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN)
	@echo "🧹 Cleaned build files."

# Run the program
run: all
	@echo "🚀 Running program..."
	./$(BIN)

.PHONY: all clean run