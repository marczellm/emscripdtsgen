cmake_minimum_required(VERSION 3.20)
project(emscripdtsgen)

set(CXX_STANDARD 17)

include_directories(../src)

# List all the files containing Embind bindings in a variable, for reuse.
# Make sure that the code uses binding_api.hpp in place of emscripten/bind.h. 
# By default it just redirects to emscripten/bind.h.
set(
    BINDING_FILES

    bindings.cpp
)

## Basic example

# Your main Emscripten binary that you plan to use from TypeScript
add_executable(
    Module

    # Suppose that you have an existing C++ library that you want to bind. 
    # List the library sources and the binding files here
    lib/lib.cpp
    ${BINDING_FILES}
)

target_link_libraries(Module PUBLIC embind)

# Add a separate executable to generate the type declarations
add_executable(
    ModuleDeclarations 
    
    ../src/emscripdtsgen.cpp
    
    # Note that you don't have to list the lib/ sources here, only the bindings
    ${BINDING_FILES}
)

# This definition redirects binding_api.hpp to the drop-in replacement header file
target_compile_definitions(ModuleDeclarations PUBLIC EMSCRIPDTSGEN)

target_link_options(
    ModuleDeclarations
    PUBLIC

    # By silencing the linker we can omit the lib/ sources from this executable, and use only the headers, which makes it build faster.
    -sWARN_ON_UNDEFINED_SYMBOLS=0
    -sNODERAWFS
)

# As a final step, run the additional executable at buildtime.
# CMake makes it easy to run one built executable as a build step of another.
# (The Emscripten CMake toolchain usefully sets CMAKE_CROSSCOMPILING_EMULATOR to the built in node.js)
add_custom_command(TARGET Module POST_BUILD COMMAND ModuleDeclarations BYPRODUCTS Module.d.ts)

## Example with MODULARIZE

# If you use MODULARIZE, EXPORT_ES6 and/or EXPORT_NAME,
# make sure to set the same settings as compile time definitions for the emscripdtsgen executable as well

add_executable(ModuleFactory lib/lib.cpp ${BINDING_FILES})
target_link_libraries(ModuleFactory PUBLIC embind)
target_link_options(ModuleFactory PUBLIC -sMODULARIZE -sEXPORT_NAME=ModuleFactory)

add_executable(ModuleFactoryDeclarations ../src/emscripdtsgen.cpp ${BINDING_FILES})
target_compile_definitions(ModuleFactoryDeclarations PUBLIC EMSCRIPDTSGEN MODULARIZE EXPORT_NAME="ModuleFactory")
target_link_options(ModuleFactoryDeclarations PUBLIC -sWARN_ON_UNDEFINED_SYMBOLS=0 -sNODERAWFS)
add_custom_command(TARGET ModuleFactory POST_BUILD COMMAND ModuleFactoryDeclarations BYPRODUCTS ModuleFactory.d.ts)

## Example with EXPORT_ES6

add_executable(ES6ModuleFactory lib/lib.cpp ${BINDING_FILES})
target_link_libraries(ES6ModuleFactory PUBLIC embind)
target_link_options(ES6ModuleFactory PUBLIC -sMODULARIZE -sEXPORT_NAME=ES6ModuleFactory)
set_target_properties(ES6ModuleFactory PROPERTIES SUFFIX ".mjs")

add_executable(ES6ModuleFactoryDeclarations ../src/emscripdtsgen.cpp ${BINDING_FILES})
target_compile_definitions(ES6ModuleFactoryDeclarations PUBLIC EMSCRIPDTSGEN MODULARIZE EXPORT_ES6 EXPORT_NAME="ES6ModuleFactory")
target_link_options(ES6ModuleFactoryDeclarations PUBLIC -sWARN_ON_UNDEFINED_SYMBOLS=0 -sNODERAWFS)
add_custom_command(TARGET ES6ModuleFactory POST_BUILD COMMAND ES6ModuleFactoryDeclarations BYPRODUCTS ES6ModuleFactory.d.mts)

