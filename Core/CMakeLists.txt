file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cc src/*.cpp include/*.h)

add_library(Core ${srcs}
        include/Core/image-io.h)
add_executable(rdcraft src/main.cpp)
target_include_directories(Core PUBLIC include)
target_include_directories(Core PUBLIC ${RENDERCRAFT_EXTERNAL}/glm)
target_include_directories(Core PUBLIC ${RENDERCRAFT_EXTERNAL}/pugixml/src)
target_include_directories(Core PUBLIC ${RENDERCRAFT_EXTERNAL}/tinyexr)
target_link_libraries(Core PUBLIC pugixml tinyobjloader Spatify tinyexr)
target_link_libraries(rdcraft PUBLIC Core TBB:tbb)