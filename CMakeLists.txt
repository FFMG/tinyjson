cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

project ("TinyJSON"
  VERSION 0.0.1
  DESCRIPTION "Very fast JSON Parser"
  LANGUAGES CXX )

add_subdirectory(external)
include(./cmake/updatesubmodules.cmake)  
  
enable_testing()

# BUILD_TESTING variable is created by include(CTest)
# It is set to ON by default
if (BUILD_TESTING)
    add_executable(tinyjsontest tests/main.cpp)
    target_link_libraries(tinyjsontest PRIVATE GTest::gtest_main)

    include(GoogleTest)
    # Finds all the Google tests associated with the executable
    gtest_discover_tests(tinyjsontest)
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_executable (TinyJSON "main.cpp" )

