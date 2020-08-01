
call emcc -O3 ./src/cpp/main.cpp -o ./src/web/SIA.js -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','cwrap','getValue'] -s EXPORTED_FUNCTIONS=['_malloc','_main']
rmdir /s /q build
mkdir build
xcopy .\src\website .\build
