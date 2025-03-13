# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./vendor -I./handlers -I./server -pthread

# Targets
SERVER_EXE = server_app
TEST_EXE = test_runner
GTEST_LIBS = -lgtest -lgtest_main -pthread

# Source files
SERVER_SRC = main.cpp handlers/QueryHandler.cpp server/server.cpp
SERVER_OBJS = $(SERVER_SRC:.cpp=.o)

TEST_SRC = unitTests.cpp handlers/QueryHandler.cpp
TEST_OBJS = $(TEST_SRC:.cpp=.o)

# Default target (build the server executable)
all: $(SERVER_EXE) $(TEST_EXE)

# Compile and link the server application
$(SERVER_EXE): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $(SERVER_EXE) $(SERVER_OBJS) -lpistache

# Compile and link the test executable
$(TEST_EXE): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_EXE) $(TEST_OBJS) -lpistache $(GTEST_LIBS)

# Compile each .cpp file separately into an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
test: $(TEST_EXE)
	./$(TEST_EXE)

# Run the server
run: $(SERVER_EXE)
	./$(SERVER_EXE)

# Clean up build files
clean:
	rm -f $(SERVER_EXE) $(TEST_EXE) $(SERVER_OBJS) $(TEST_OBJS)
