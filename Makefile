# Build Directories
BUILD_DEBUG_STATIC   := build/debug_static
BUILD_DEBUG_SHARED   := build/debug_shared
BUILD_RELEASE_STATIC := build/release_static
BUILD_RELEASE_SHARED := build/release_shared

CMAKE_ARGS           :=

CORES                ?= $(shell nproc 2>/dev/null || echo 4)

.PHONY: all debug release shared-debug shared-release runt rune clean clean-full help

all: debug

$(BUILD_DEBUG_STATIC)/CMakeCache.txt: CMakeLists.txt
	@echo "Configuring Debug Static build..."
	@mkdir -p $(BUILD_DEBUG_STATIC)
	@cmake -S . -B $(BUILD_DEBUG_STATIC) -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=OFF $(CMAKE_ARGS)

$(BUILD_DEBUG_SHARED)/CMakeCache.txt: CMakeLists.txt
	@echo "Configuring Debug Shared build..."
	@mkdir -p $(BUILD_DEBUG_SHARED)
	@cmake -S . -B $(BUILD_DEBUG_SHARED) -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON $(CMAKE_ARGS)

$(BUILD_RELEASE_STATIC)/CMakeCache.txt: CMakeLists.txt
	@echo "Configuring Release Static build..."
	@mkdir -p $(BUILD_RELEASE_STATIC)
	@cmake -S . -B $(BUILD_RELEASE_STATIC) -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF $(CMAKE_ARGS)

$(BUILD_RELEASE_SHARED)/CMakeCache.txt: CMakeLists.txt
	@echo "Configuring Release Shared build..."
	@mkdir -p $(BUILD_RELEASE_SHARED)
	@cmake -S . -B $(BUILD_RELEASE_SHARED) -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON $(CMAKE_ARGS)

debug: $(BUILD_DEBUG_STATIC)/CMakeCache.txt
	@echo "Building Debug Static..."
	@cmake --build $(BUILD_DEBUG_STATIC) -j $(CORES)

shared-debug: $(BUILD_DEBUG_SHARED)/CMakeCache.txt
	@echo "Building Debug Shared..."
	@cmake --build $(BUILD_DEBUG_SHARED) -j $(CORES)

release: $(BUILD_RELEASE_STATIC)/CMakeCache.txt
	@echo "Building Release Static..."
	@cmake --build $(BUILD_RELEASE_STATIC) -j $(CORES)

shared-release: $(BUILD_RELEASE_SHARED)/CMakeCache.txt
	@echo "Building Release Shared..."
	@cmake --build $(BUILD_RELEASE_SHARED) -j $(CORES)

runt: debug
	@if [ -z "$(NAME)" ]; then echo "Specify NAME"; exit 1; fi
	@echo "Running Test '$(NAME)'..."
	@./$(BUILD_DEBUG_STATIC)/bin/test_$(NAME)

rune: debug
	@if [ -z "$(NAME)" ]; then echo "Specify NAME"; exit 1; fi
	@echo "Running Example '$(NAME)'..."
	@./$(BUILD_DEBUG_STATIC)/bin/example_$(NAME)

clean:
	@echo "Cleaning standard build outputs..."
	@rm -rf $(BUILD_DEBUG_STATIC)/test $(BUILD_DEBUG_STATIC)/example
	@rm -rf $(BUILD_DEBUG_SHARED)/test $(BUILD_DEBUG_SHARED)/example

clean-full:
	@echo "Cleaning build directory..."
	@rm -rf build

help:
	@echo "Usage:"
	@echo "  make                 - Build Debug Static version"
	@echo "  make debug           - Build Debug Static version"
	@echo "  make release         - Build Release Static version"
	@echo "  make shared-debug    - Build Debug Shared version"
	@echo "  make shared-release  - Build Release Shared version"
	@echo "  make runt NAME=x     - Build and run test_x (debug)"
	@echo "  make rune NAME=x     - Build and run example_x (debug)"
	@echo "  make clean [NAME=x]  - Remove local object files"
	@echo "  make clean-full      - Remove all build files"