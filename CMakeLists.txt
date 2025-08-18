# CMakeLists.txt for the Clue project

cmake_minimum_required(VERSION 3.27)

project(Clue)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Source)
add_subdirectory(ThirdParty)