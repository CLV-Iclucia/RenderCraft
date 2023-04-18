cmake_minimum_required(VERSION 3.23)
project(RenderCraft VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(RENDERCRAFT_EXTERNAL "${PROJECT_SOURCE_DIR}/external")
set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/external")

message("Finding MX3d ...")
find_package(MX3d CONFIG)

if(MX3d_FOUND)
    message("MX3d found at ${MX3d_DIR}")
else()
    message(FATAL_ERROR "MX3d not found. Use \"git submodule --init --update\" to clone the missing submodules.")
endif()

add_subdirectory(${RENDERCRAFT_EXTERNAL}/pugixml)

file(GLOB RENDERCRAFT_SRC ${PROJECT_SOURCE_DIR}/src/render/* ${PROJECT_SOURCE_DIR}/src/utility/*)
add_executable(RenderCraft ${RENDERCRAFT_SRC} ${XMATH_CORE} ${XMATH_EXT})
