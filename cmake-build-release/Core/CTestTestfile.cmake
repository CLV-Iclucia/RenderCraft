# CMake generated Testfile for 
# Source directory: /home/creeper/CLionProjects/RenderCraft/Core
# Build directory: /home/creeper/CLionProjects/RenderCraft/cmake-build-release/Core
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[test-bvh]=] "/home/creeper/CLionProjects/RenderCraft/cmake-build-release/Core/test-bvh")
  set_tests_properties([=[test-bvh]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/creeper/CLionProjects/RenderCraft/Core/CMakeLists.txt;21;add_test;/home/creeper/CLionProjects/RenderCraft/Core/CMakeLists.txt;0;")
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[test-parser]=] "/home/creeper/CLionProjects/RenderCraft/cmake-build-release/Core/test-parser")
  set_tests_properties([=[test-parser]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/creeper/CLionProjects/RenderCraft/Core/CMakeLists.txt;21;add_test;/home/creeper/CLionProjects/RenderCraft/Core/CMakeLists.txt;0;")
endif()
