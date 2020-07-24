#---------------------------------------------------------------------------
# Cmake config
#---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0)


#---------------------------------------------------------------------------
# Project Config
#---------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message (FATAL_ERROR "In-source builds are not permitted; run CMake inside an empty build directory.")
endif ()

project(filianoremaya)

set (CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${PROJECT_SOURCE_DIR}/cmake/modules)


#---------------------------------------------------------------------------
# Find external Pkgs
#---------------------------------------------------------------------------

find_package (filianore REQUIRED)
if(filianore_FOUND)
  message("Filianore Lib found.")
else()
  message(FATAL_ERROR "Filianore Lib not found.")
endif()

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)


#---------------------------------------------------------------------------
# Main project build
#---------------------------------------------------------------------------

set(SOURCE_FILES 
  src/filianoremaya/util.cpp
  src/filianoremaya/pluginmain.cpp)

set(LIBRARIES OpenMaya Foundation)

build_plugin()

target_link_libraries (filianoremaya filianore)


# file(COPY ${CMAKE_BUILD_DIRECTORY}/filianoremaya.bundle
#      DESTINATION /Users/edem/Documents/dev/filianore-maya/plug-ins)
