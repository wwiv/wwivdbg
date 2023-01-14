cmake_minimum_required(VERSION 3.15)

#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
#  CACHE STRING "Vcpkg toolchain file")

 set(CMAKE_TOOLCHAIN_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(wwivdbg CXX)
set (CMAKE_CXX_STANDARD 20)

find_package(fmt CONFIG REQUIRED)
find_package(tvision CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)

add_library(tvcommon 
  "src/tvcommon/datapane.cpp"
  "src/tvcommon/listwindow.cpp"
)
target_compile_features(tvcommon PRIVATE cxx_std_17)
target_link_libraries(tvcommon PRIVATE 
  fmt::fmt 
  tvision::tvision
)

add_executable(wwivdbg 
  "src/breakpoints.cpp" 
  "src/breakpoints2.cpp" 
  "src/editor.cpp"
  "src/menu.cpp"
  "src/protocol.cpp" 
  "src/source.cpp"
  "src/stack.cpp" 
  "src/strings.cpp" 
  "src/utils.cpp"
  "src/vars.cpp" 
  "src/wwivdbg.cpp")
target_compile_features(wwivdbg PRIVATE cxx_std_17)

target_link_libraries(wwivdbg PRIVATE 
  fmt::fmt 
  httplib::httplib
  nlohmann_json::nlohmann_json
  tvision::tvision
  tvcommon
)
