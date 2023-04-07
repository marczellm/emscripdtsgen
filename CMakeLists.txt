cmake_minimum_required(VERSION 3.20)
project(emscripdtsgen)

set(CXX_STANDARD 17)

add_executable(Module lib.cpp bindings.cpp)
target_link_libraries(Module PUBLIC embind)

add_executable(ModuleDeclarations emscripdtsgen.cpp bindings.cpp)
target_compile_definitions(ModuleDeclarations PUBLIC EMSCRIPDTSGEN)
target_link_options(ModuleDeclarations PUBLIC -sWARN_ON_UNDEFINED_SYMBOLS=0)
add_custom_command(OUTPUT Module.d.ts COMMAND ModuleDeclarations)
add_custom_target(GenerateModuleDeclarations DEPENDS Module.d.ts)
add_dependencies(Module GenerateModuleDeclarations)

add_executable(ModuleFactory lib.cpp bindings.cpp)
target_link_libraries(ModuleFactory PUBLIC embind)
target_link_options(ModuleFactory PUBLIC -sMODULARIZE -sEXPORT_NAME=ModuleFactory)

add_executable(ModuleFactoryDeclarations emscripdtsgen.cpp bindings.cpp)
target_compile_definitions(ModuleFactoryDeclarations PUBLIC EMSCRIPDTSGEN EXPORT_NAME="ModuleFactory")
target_link_options(ModuleFactoryDeclarations PUBLIC -sWARN_ON_UNDEFINED_SYMBOLS=0)
add_custom_command(OUTPUT ModuleFactory.d.ts COMMAND ModuleFactoryDeclarations)
add_custom_target(GenerateModuleFactoryDeclarations DEPENDS ModuleFactory.d.ts)
add_dependencies(Module GenerateModuleFactoryDeclarations)

add_executable(ES6ModuleFactory lib.cpp bindings.cpp)
target_link_libraries(ES6ModuleFactory PUBLIC embind)
target_link_options(ES6ModuleFactory PUBLIC -sMODULARIZE -sEXPORT_NAME=ES6ModuleFactory)
set_target_properties(ES6ModuleFactory PROPERTIES SUFFIX ".mjs")

add_executable(ES6ModuleFactoryDeclarations emscripdtsgen.cpp bindings.cpp)
target_compile_definitions(ES6ModuleFactoryDeclarations PUBLIC EMSCRIPDTSGEN EXPORT_ES6 EXPORT_NAME="ES6ModuleFactory")
target_link_options(ES6ModuleFactoryDeclarations PUBLIC -sWARN_ON_UNDEFINED_SYMBOLS=0)
add_custom_command(OUTPUT ES6ModuleFactory.d.mts COMMAND ES6ModuleFactoryDeclarations)
add_custom_target(GenerateES6ModuleFactoryDeclarations DEPENDS ES6ModuleFactory.d.mts)
add_dependencies(Module GenerateES6ModuleFactoryDeclarations)
