cmake_minimum_required(VERSION 3.20)
project(emscripdtsgen)

add_executable(
    lib

    lib.cpp
    bindings.cpp
)

target_link_libraries(
    lib

    PUBLIC
    embind
)

# add_executable(
#     emscripdtsgen
    
#     bindings.cpp
#     emscripdtsgen.cpp
# )