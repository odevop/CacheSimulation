CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -O2 -Isrc
BUILD_DIR := build
TARGET    := $(BUILD_DIR)/CacheSimulation.exe
SRC       := src/main.cpp src/cache.cpp

main: $(TARGET)

$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
