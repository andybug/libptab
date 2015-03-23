
CMAKE_BUILD_TYPE ?= Release
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

.PHONY: all clean test install ptab

all: ptab

ptab: build/.ran-cmake
	@make -C build --no-print-directory

test: build/.ran-cmake
	@./build/bin/ptab-test

install: build/.ran-cmake
	@make install -C build --no-print-directory

clean:
	@test ! -d build || rm -r build >/dev/null 2>&1

build/.ran-cmake:
	@mkdir -p build
	@cd build && cmake $(CMAKE_FLAGS) ..
	@touch $@
