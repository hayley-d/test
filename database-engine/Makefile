MAKEFLAGS += -j$(nproc)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)
SRC_FILES_FOR_TESTS = $(filter-out $(SRC_DIR)/Main.cpp, $(SRC_FILES))

TARGET = $(BUILD_DIR)/main
TEST_TARGET = $(BUILD_DIR)/tests

OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TEST_OBJ_FILES = $(TEST_FILES:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%.o)

.PHONY: all clean test memcheck cov

all: $(TARGET)

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build target
$(TARGET): $(BUILD_DIR) $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $@

# Build test target
$(TEST_TARGET): $(SRC_FILES_FOR_TESTS) $(TEST_OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(SRC_FILES_FOR_TESTS) $(TEST_OBJ_FILES) -o $@ -lgcov

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
test: $(TEST_TARGET)
	$(BUILD_DIR)/tests

# Run server
server: $(TARGET)
	$(TARGET)

# Memory check 
memcheck: $(TEST_TARGET)
	valgrind --leak-check=full --track-origins=yes $(BUILD_DIR)/tests

# Code coverage
cov: $(TEST_TARGET)
	mkdir -p coverage
	$(BUILD_DIR)/tests
	gcov $(SRC_FILES) -o $(BUILD_DIR) -o coverage

# Clean build and object files
clean:
	rm -rf $(BUILD_DIR)/*

