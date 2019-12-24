cmake_minimum_required( VERSION 2.6.0 )

# Project name
project( curves )

# Set extra compiler flags
if( UNIX )
  set( CMAKE_CXX_FLAGS "-Wall -Werror=return-type -g" )
endif( UNIX )

# Add sources
aux_source_directory( . curves_src )

# Where to find headers
include_directories( .  curves_src )
include_directories( .. curves_src )

# Required libraries are added to this variable
set( requiredLibs )

# Where to look for extra modules used to find libraries
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake_modules/" )

# OpenGL (GL, GLU, GLEXT)
set ( OpenGL_GL_PREFERENCE "LEGACY")
find_package( OpenGL REQUIRED )
include_directories( SYSTEM ${OPENGL_INCLUDE_DIR} )
set( requiredLibs ${requiredLibs} ${OPENGL_LIBRARIES} )

# GLEW
find_package( GLEW REQUIRED )
include_directories( ${GLEW_INCLUDE_DIR} )
set( requiredLibs ${requiredLibs} ${GLEW_LIBRARIES} )

# GLM
find_package( GLM REQUIRED )
include_directories( ${GLM_INCLUDE_DIR} )
set( requiredLibs ${requiredLibs} ${GLM_LIBRARY} )

# GLFW3
find_package (GLFW3 REQUIRED glfw3)
include_directories (SYSTEM ${GLFW3_INCLUDE_DIR})
set( requiredLibs ${requiredLibs} ${GLFW3_LIBRARY})

# Create build files for executable
add_executable( curves ${curves_src} )

# Link against libraries
target_link_libraries( curves ${requiredLibs} ${optionalLibs} )

# Build type
set( CMAKE_BUILD_TYPE Debug )
