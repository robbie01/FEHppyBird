cp simulator_libraries/* ./
emcc main.cpp -s WASM=1 -o index.html
rm FEH* tigr* LCD*
