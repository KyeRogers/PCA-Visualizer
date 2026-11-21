.PHONY: configure build test clean

build/Makefile: CMakeLists.txt
	cmake -S . -B build

configure: build/Makefile

build: build/Makefile
	cmake --build build

test: build
	ctest --test-dir build --output-on-failure

clean:
	cmake -E rm -rf build