cmake_minimum_required(VERSION 3.20)
project(emscripdtsgen)

set(CXX_STANDARD 17)

add_executable(Module lib.cpp bindings.cpp)
target_link_libraries(Module PUBLIC embind)

add_executable(ModuleFactory lib.cpp bindings.cpp)
target_link_libraries(ModuleFactory PUBLIC embind)
target_link_options(ModuleFactory PUBLIC -sMODULARIZE -sEXPORT_NAME=ModuleFactory)

add_executable(ES6ModuleFactory lib.cpp bindings.cpp)
target_link_libraries(ES6ModuleFactory PUBLIC embind)
target_link_options(ES6ModuleFactory PUBLIC -sMODULARIZE -sEXPORT_NAME=ES6ModuleFactory)
set_target_properties(ES6ModuleFactory PROPERTIES SUFFIX ".mjs")

# add_executable(
#     emscripdtsgen
    
#     bindings.cpp
#     emscripdtsgen.cpp
# )