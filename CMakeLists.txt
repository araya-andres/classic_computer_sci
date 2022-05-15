cmake_minimum_required(VERSION 3.21)
project(classic_computer_sci CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(cxx-prettyprint)
include_directories(range-v3/include)

add_compile_options(
  -g
  -pedantic
  -pedantic-errors
# FIXME
#   -Wall
#   -Wextra
#   -Werror
#   -Wconversion
  )

add_subdirectory(ch1)
add_subdirectory(ch2)
add_subdirectory(ch3)
add_subdirectory(ch4)
add_subdirectory(ch5)