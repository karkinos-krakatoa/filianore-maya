#---------------------------------------------------------------------------
# Cmake config
#---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.14...3.25)

project(filianoremaya
    DESCRIPTION "Maya plugin for Filianore")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#---------------------------------------------------------------------------
# Project Config
#---------------------------------------------------------------------------

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message (FATAL_ERROR "In-source builds are not permitted; run CMake inside an empty build directory.")
endif ()


#---------------------------------------------------------------------------
# Include Maya Devkit
#---------------------------------------------------------------------------

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)


#---------------------------------------------------------------------------
# Include Filianore-Core by default
#---------------------------------------------------------------------------

add_subdirectory(external/filianore)


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
  src/filianoremaya/materialprocessorutil.cpp
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

build_plugin()

target_link_libraries (filianoremaya filianore)