cmake_minimum_required(VERSION 3.23)
project(RenderCraft VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(RENDERCRAFT_EXTERNAL "${PROJECT_SOURCE_DIR}/external")
set(RENDERCRAFT_SCENE_DIR "${PROJECT_SOURCE_DIR}/scenes")
set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/external")

find_package(TBB REQUIRED)
add_subdirectory(${RENDERCRAFT_EXTERNAL}/pugixml)
add_subdirectory(${RENDERCRAFT_EXTERNAL}/tinyobjloader)
add_subdirectory(${RENDERCRAFT_EXTERNAL}/tinyexr)
add_subdirectory(Spatify)
add_subdirectory(Core)