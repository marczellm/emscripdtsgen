cmake_minimum_required(VERSION 3.20)
project(emscripdtsgen)

set(CXX_STANDARD 17)

add_executable(Module lib.cpp bindings.cpp)
target_link_libraries(Module PUBLIC embind)

add_executable(ModuleDeclarations emscripdtsgen.cpp bindings.cpp)
target_compile_definitions(ModuleDeclarations PUBLIC EMSCRIPDTSGEN)
target_link_options(ModuleDeclarations PUBLIC -sWARN_ON_UNDEFINED_SYMBOLS=0 -sNODERAWFS)
# add_custom_command(TARGET Module POST_BUILD COMMAND ModuleDeclarations BYPRODUCTS Module.d.ts)

add_executable(ModuleFactory lib.cpp bindings.cpp)
target_link_libraries(ModuleFactory PUBLIC embind)
target_link_options(ModuleFactory PUBLIC -sMODULARIZE -sEXPORT_NAME=ModuleFactory)

add_executable(ModuleFactoryDeclarations emscripdtsgen.cpp bindings.cpp)
target_compile_definitions(ModuleFactoryDeclarations PUBLIC EMSCRIPDTSGEN MODULARIZE EXPORT_NAME="ModuleFactory")
target_link_options(ModuleFactoryDeclarations PUBLIC -sWARN_ON_UNDEFINED_SYMBOLS=0 -sNODERAWFS)
# add_custom_command(TARGET ModuleFactory POST_BUILD COMMAND ModuleFactoryDeclarations BYPRODUCTS ModuleFactory.d.ts)

add_executable(ES6ModuleFactory lib.cpp bindings.cpp)
target_link_libraries(ES6ModuleFactory PUBLIC embind)
target_link_options(ES6ModuleFactory PUBLIC -sMODULARIZE -sEXPORT_NAME=ES6ModuleFactory)
set_target_properties(ES6ModuleFactory PROPERTIES SUFFIX ".mjs")

add_executable(ES6ModuleFactoryDeclarations emscripdtsgen.cpp bindings.cpp)
target_compile_definitions(ES6ModuleFactoryDeclarations PUBLIC EMSCRIPDTSGEN MODULARIZE EXPORT_ES6 EXPORT_NAME="ES6ModuleFactory")
target_link_options(ES6ModuleFactoryDeclarations PUBLIC -sWARN_ON_UNDEFINED_SYMBOLS=0 -sNODERAWFS)
add_custom_command(TARGET ES6ModuleFactory POST_BUILD COMMAND ES6ModuleFactoryDeclarations BYPRODUCTS ES6ModuleFactory.d.mts)

