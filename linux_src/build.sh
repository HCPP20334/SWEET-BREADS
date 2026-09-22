#!/bin/bash
set -e
CXX=g++
CXXFLAGS="-std=c++20 -O3 -Wall -Wno-unused-result -Wunused-variable -Wformat"
SOURCES="
main.cpp
imgui.cpp
imgui_draw.cpp
imgui_tables.cpp
imgui_widgets.cpp
imgui_stdlib.cpp
imgui_impl_sdl2.cpp
imgui_impl_opengl2.cpp
"

INCLUDES="-I. $(sdl2-config --cflags)"
LIBS="-lGL -ldl -lpthread $(sdl2-config --libs)"

OUTPUT="PahomEngineSDL2"

echo "Building $OUTPUT..."
$CXX $CXXFLAGS $INCLUDES $SOURCES $LIBS -o $OUTPUT
echo "Build complete: ./$OUTPUT"
