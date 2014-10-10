
CMAKE_BUILD_TYPE ?= Debug
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

.PHONY: all libptables clean

all: libptables

libptables: build/.ran-cmake
	@make -C build --no-print-directory

build/.ran-cmake:
	mkdir -p build
	cd build && cmake $(CMAKE_FLAGS) ..
	touch $@

clean:
	rm -r build

