cmake_minimum_required( VERSION 2.6.0 )

# Project name
project( trackball )

# Set extra compiler flags
if( UNIX )
  set( CMAKE_CXX_FLAGS "-Wall -Werror=return-type -g" )
endif( UNIX )

# Add sources
aux_source_directory( . trackball_src )

# Where to find headers
include_directories( .  trackball_src )
include_directories( .. trackball_src )

# Required libraries are added to this variable
set( requiredLibs )

# Where to look for extra modules used to find libraries
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake_modules/" )

# OpenGL (GL, GLU, GLEXT)
find_package( OpenGL REQUIRED )
include_directories( SYSTEM ${OPENGL_INCLUDE_DIR} )
set( requiredLibs ${requiredLibs} ${OPENGL_LIBRARIES} )

# GLEW
if( WIN32 )
  find_library( GLEW_LIBRARIES glew32.lib )
  set( requiredLibs ${requiredLibs} ${GLEW_LIBRARIES} )
endif( WIN32 )
if( UNIX )
  find_package( GLEW REQUIRED )
  include_directories( ${GLEW_INCLUDE_DIR} )
  set( requiredLibs ${requiredLibs} ${GLEW_LIBRARY} )
endif( UNIX )

# GLM
find_package( GLM REQUIRED )
include_directories( ${GLM_INCLUDE_DIR} )
set( requiredLibs ${requiredLibs} ${GLM_LIBRARY} )


# GLFW3
find_package (GLFW3 REQUIRED glfw3)
include_directories (SYSTEM ${GLFW3_INCLUDE_DIR})
set( requiredLibs ${requiredLibs} ${GLFW3_LIBRARY})

# lodepng
#aux_source_directory( ../libs/lodepng trackball_src )
#include_directories(SYSTEM ../libs/lodepng )

# AntTweakBar
#include_directories(SYSTEM ../libs/AntTweakBar/include )
#link_directories( ../libs/AntTweakBar/lib )
#set( requiredLibs ${requiredLibs} AntTweakBar )

# Create build files for executable
add_executable( trackball ${trackball_src} )

# Link against libraries
target_link_libraries( trackball ${requiredLibs} ${optionalLibs} )

# Build type
set( CMAKE_BUILD_TYPE Debug )
