rmdir /s /q build
cmake -S . -B build
cmake --build build
build\Debug\CellularAutomata.exe