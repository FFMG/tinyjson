cmake_minimum_required (VERSION 3.28)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(Project TinyJSONTests)

include(GoogleTest)

project (${Project}
  VERSION 0.0.1
  DESCRIPTION "TinyJSON Tests"
  LANGUAGES CXX C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set( Headers
)

set( Sources
  "testtinyjson.cpp"
  "testtinyjsonarrays.cpp"
  "testtinyjsonbooleans.cpp"
  "testtinyjsonexponents.cpp"
  "testtinyjsonnulls.cpp"
  "testtinyjsonnumbers.cpp"
  "testtinyjsonobjects.cpp"
  "testtinyjsonstrings.cpp"
)

add_executable(
  ${Project}
  ${Headers}
  ${Sources})
target_link_libraries(${Project} PUBLIC gtest_main TinyJSON)

# Finds all the Google tests associated with the executable
gtest_discover_tests(${Project})