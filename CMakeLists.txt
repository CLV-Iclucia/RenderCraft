cmake_minimum_required(VERSION 3.23)
project(RenderCraft)

set(CMAKE_CXX_STANDARD 17)

file(GLOB RENDER_SRC src/*)
add_executable(RenderCraft ${RENDER_SRC} src/Lights.cpp src/global.h)
