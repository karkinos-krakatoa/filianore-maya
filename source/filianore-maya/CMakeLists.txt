#---------------------------------------------------------------------------
# Cmake config
#---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.14)


#---------------------------------------------------------------------------
# Project Config
#---------------------------------------------------------------------------

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

if (NOT MSVC)
    find_package(OpenMP REQUIRED)
    if (OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found")      
    endif ()
else ()
    message(STATUS "OpenMP is disabled because MSVC only supports OpenMP 2.0.")
endif ()

find_package (filianore REQUIRED)
if(filianore_FOUND)
  message("Filianore Foundation Lib found.")
else()
  message(FATAL_ERROR "Filianore Foundation Lib not found.")
endif()

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)


#---------------------------------------------------------------------------
# Main project build
#---------------------------------------------------------------------------

set(SOURCE_FILES 
  src/filianoremaya/util.cpp
  src/filianoremaya/dagUtils.cpp
  src/filianoremaya/cameraexporter.cpp
  src/filianoremaya/meshexporter.cpp
  src/filianoremaya/illuminantsexporter.cpp
  src/filianoremaya/renderglobalsnode.cpp
  src/filianoremaya/flplasticshaderoverride.cpp
  src/filianoremaya/flplasticshader.cpp
  src/filianoremaya/flstandardsurfaceshaderoverride.cpp
  src/filianoremaya/flstandardsurfaceshader.cpp
  src/filianoremaya/materialprocessor.cpp
  src/filianoremaya/finalrendercommand.cpp
  src/filianoremaya/pluginmain.cpp
  )

set(LIBRARIES 
     OpenMayaRender
     OpenMayaUI
     OpenMaya
     Foundation
     TBB)

set(CMAKE_CXX_STANDARD 17)

build_plugin()

target_link_libraries (filianoremaya filianore)
target_link_libraries (filianoremaya OpenMP::OpenMP_CXX)