# Emscripdtsgen
Emscripdtsgen is a tool to generate [TypeScript typing declarations](https://www.typescriptlang.org/docs/handbook/2/type-declarations.html#dts-files) from [Embind](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html) bindings.
## How it works
Emscripdtsgen provides a drop-in replacement header file for `emscripten/bind.h`. You compile a separate executable using this in your binding files. Running this executable will output your type declarations.
## Getting started
Please see comments in `example/CMakeLists.txt` for a step by step guide.
### Building and running the examples
```
npm install
emcmake cmake -S examples -B build -G Ninja
cmake --build build --target all
cd examples/basic
npx tsc
cd ../modularize
npx tsc
cd ../es6
npx tsc
```

Start a simple http server in the root folder, like
```
python -m http.server
```
and then visit
```
http://localhost:8000/examples/basic/
http://localhost:8000/examples/modularize/
http://localhost:8000/examples/es6/
```
## Incompleteness
As with so many things on GitHub, Emscripdtsgen is incomplete, and I'm not even promising to complete or maintain it. I offer it as a potentially useful starting point from which you can build out your tooling.