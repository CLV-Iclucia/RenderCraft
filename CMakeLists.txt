cmake_minimum_required(VERSION 3.23)
project(RenderCraft VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(RENDERCRAFT_EXTERNAL "${PROJECT_SOURCE_DIR}/external")
set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/external")

add_subdirectory(${RENDERCRAFT_EXTERNAL}/pugixml)
add_subdirectory(Core)

