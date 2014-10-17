
CMAKE_BUILD_TYPE ?= Debug
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

.PHONY: all clean test libptab valgrind

all: libptab

libptab: build/.ran-cmake
	@make -C build --no-print-directory

build/.ran-cmake:
	mkdir -p build
	cd build && cmake $(CMAKE_FLAGS) ..
	touch $@

test:
	@make -C build --no-print-directory test

valgrind:
	@echo -e '\x1b[36mRunning Valgrind...\x1b[39;49m'
	@./scripts/valgrind.sh > /dev/null 2>&1
	@echo 'Passed'

clean:
	rm -r build

