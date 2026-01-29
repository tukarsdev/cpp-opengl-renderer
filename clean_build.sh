rm -rf build
cmake -B build
cmake --build build --config Debug
./build/Debug/renderer.exe