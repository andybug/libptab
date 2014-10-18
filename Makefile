
CMAKE_BUILD_TYPE ?= Debug
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

.PHONY: all clean test libptab

all: libptab

libptab: build/.ran-cmake
	@make -C build --no-print-directory

build/.ran-cmake:
	@mkdir -p build
	@cd build && cmake $(CMAKE_FLAGS) ..
	@touch $@

test:
	@make -C build --no-print-directory test

clean:
	@test ! -d build || rm -r build >/dev/null 2>&1; fi

