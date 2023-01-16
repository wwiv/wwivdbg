cmake_minimum_required(VERSION 3.15)

#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
#  CACHE STRING "Vcpkg toolchain file")

 set(CMAKE_TOOLCHAIN_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(wwivdbg CXX)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
# Work around https://gitlab.kitware.com/cmake/cmake/-/issues/24209 util cmake 3.25.1
set (CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0)

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

option(WWIVDBG_INSTALL "Create install packages for both zip files and binaries." ON)

############################################################################### 
#
# Build variables to come from Jenkins, environment, etc.

set(WWIVDBG_RELEASE "1.0.0" CACHE STRING "WWIVdbg Release Major Version to 3 digits")
set(WWIVDBG_BUILD_NUMBER "development" CACHE STRING "WWIVdbg Build Number")
set(WWIVDBG_FULL_RELEASE "${WWIVDBG_RELEASE}.${WWIVDBG_BUILD_NUMBER}" CACHE INTERNAL "WWIVdbg Release Version, 4 digits with build number as 4th")
set(WWIVDBG_ARCH "x86" CACHE STRING "x86 or x64")
set(WWIVDBG_DISTRO "unknown" CACHE STRING "WWIVdbg OS Distribution e.g. (win-x86 | linux-debian10) ")
set(CPACK_PACKAGE_FILE_NAME "wwiv-${WWIVDBG_DISTRO}-${WWIVDBG_FULL_RELEASE}")

set(WWIVDBG_INSTALL_SRC "${CMAKE_SOURCE_DIR}/install" CACHE STRING "By default this is: ${CMAKE_SOURCE_DIR}/install")
set(WWIVDBG_RELEASE_DIR "${CMAKE_BINARY_DIR}/release" CACHE STRING "By default this is: ${CMAKE_BINARY_DIR}/release")
file(MAKE_DIRECTORY ${WWIVDBG_RELEASE_DIR})
#set(MY_CACHE_VARIABLE "VALUE" CACHE STRING "Description")

# Packaging support
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_PACKAGE_NAME "WWIV")
set(CPACK_PACKAGE_VENDOR "WWIV Software Services")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "WWIVbasic Debugger, for use with WWIV Bulleting Board System v5.8 or later.")

string(REPLACE "." ";" VERSION_LIST ${WWIVDBG_RELEASE})
list(GET VERSION_LIST 0 CPACK_PACKAGE_VERSION_MAJOR)
list(GET VERSION_LIST 1 CPACK_PACKAGE_VERSION_MINOR)
list(GET VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH)

message(STATUS "Set CPACK_PACKAGE_FILE_NAME: ${CPACK_PACKAGE_FILE_NAME}")
message(STATUS "Set CPACK_PACKAGE_VERSION: ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")

file(TO_NATIVE_PATH "C:/wwiv" C_WWIVDBG_PATH)
set(CPACK_NSIS_INSTALL_ROOT "${C_WWIVDBG_PATH}")

set(CPACK_NSIS_PACKAGE_NAME "WWIVdbg Debugger")
set(CPACK_NSIS_URL_INFO_ABOUT "http://www.wwivbbs.org")
set(CPACK_NSIS_CONTACT "http://docs.wwivbbs.org")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "")
include(CPack)


message(STATUS "WWIVdbg Build Number: ${WWIVDBG_FULL_RELEASE}")

if (WWIVDBG_INSTALL)
  # Create build.nfo
  message(STATUS "Writing ${CMAKE_BINARY_DIR}/BUILD.NFO")
  file(
    WRITE "${CMAKE_BINARY_DIR}/build.nfo"
    "Build URL $ENV{BUILD_URL}\n"
    "Build Version: $ENV{BUILD_NUMBER}\n\n"
  )

  install(TARGETS wwivdbg DESTINATION .)

  file(GLOB DOCS_FILES "${WWIVDBG_INSTALL_SRC}/docs/*")
  foreach(file ${DOCS_FILES})
    INSTALL(FILES "${file}" DESTINATION .)
  endforeach()
endif (WWIVDBG_INSTALL)
