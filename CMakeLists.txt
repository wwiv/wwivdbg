cmake_minimum_required(VERSION 3.15)

#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
#  CACHE STRING "Vcpkg toolchain file")

 set(CMAKE_TOOLCHAIN_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(wwivdbg CXX)

find_package(fmt CONFIG REQUIRED)
find_package(tvision CONFIG REQUIRED)

add_executable(wwivdbg src/wwivdbg.cpp)
target_compile_features(wwivdbg PRIVATE cxx_std_17)

target_link_libraries(wwivdbg PRIVATE fmt::fmt tvision::tvision)