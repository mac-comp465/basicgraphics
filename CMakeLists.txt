set (CMAKE_CXX_STANDARD 11)

# Using 3.9 to get a modern version of FindOpenGL.cmake
cmake_minimum_required (VERSION 3.10) 

project(BasicGraphics)

# !!!!!!!!!!!!! EDIT THE FOLLOWING LINE AS NEEDED !!!!!!!!!!!!! 
list(APPEND CMAKE_PREFIX_PATH ../minvr/build/install)

# Dependencies that are auto-downloaded, built, and installed for you will go here.  So, you 
# may use this even if not planning to install this particular project on your system.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

list(APPEND CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules ${CMAKE_INSTALL_PREFIX})
list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_INSTALL_PREFIX})

include(AutoBuild)
AutoBuild_init()

# These are relative to ${CMAKE_INSTALL_PREFIX}
set(INSTALL_BIN_DEST           bin)
set(INSTALL_LIB_DEST           lib)
set(INSTALL_INCLUDE_DEST       include/BasicGraphics)
set(INSTALL_CMAKECONFIG_DEST   lib/cmake/BasicGraphics)

set (CMAKE_VERBOSE_MAKEFILE TRUE)

message(STATUS "==== BUILDING ${PROJECT_NAME}")
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")

#---------------------- Source for this Project ----------------------

set (source_files
 src/Mesh.cpp
 src/Model.cpp
 src/GLSLProgram.cpp
 src/Texture.cpp
 src/Line.cpp
 src/Sphere.cpp
 src/Box.cpp
 src/Cylinder.cpp
 src/Cone.cpp
# src/glad/src/glad.c
)

set (header_files
 src/BasicGraphics.h
 src/Mesh.h
 src/Model.h
 src/GLSLProgram.h
 src/Texture.h
 src/Line.h
 src/Sphere.h
 src/Box.h
 src/Cylinder.h
 src/Cone.h
 src/fontstash.h
 src/glfontstash.h
 src/stb_truetype.h 
)

source_group("Header Files" FILES ${header_files})

# Organize folder structure for Xcode, Visual Studio, etc.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#---------------------- Define the Target ----------------------

add_library(${PROJECT_NAME} ${source_files} ${header_files})


#---------------------- Find & Add Dependencies ----------------------

# MinVR (linked with an imported cmake target so no need to specify include dirs)
#find_package(MinVR REQUIRED)
#target_link_libraries(${PROJECT_NAME} PUBLIC MinVR::MinVR)

if (APPLE)
  #exec_program(xcrun ARGS "--sdk macosx --show-sdk-path" OUTPUT_VARIABLE STDLIB_SDK_PATH)
  
  #exec_program(xcrun ARGS "--show-sdk-path" OUTPUT_VARIABLE STDLIB_SDK_PATH)
  #message("SDK_PATH IS ${STDLIB_SDK_PATH}")
  #list(APPEND CMAKE_PREFIX_PATH ${STDLIB_SDK_PATH}/usr)
  #set(CMAKE_OSX_SYSROOT ${STDLIB_SDK_PATH})
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot ${STDLIB_SDK_PATH}")
  
  #get_filename_component(TOOLCHAIN_PATH ${CMAKE_LINKER} DIRECTORY)
  #message(STATUS "Toolchain include path: ${TOOLCHAIN_PATH}/../include")
  #include_directories(BEFORE SYSTEM
  #    "${TOOLCHAIN_PATH}/../include/"
  #)
  add_definitions( -DGL_SILENCE_DEPRECATION )
endif()


# OpenGL
include(AutoBuildOpenGL)
AutoBuild_use_package_OpenGL(${PROJECT_NAME} PUBLIC)


# GLM
#AutoBuild_use_package(glm . ${PROJECT_NAME} PUBLIC glm
#    DOWNLOAD_ARGS  GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG master #BUILD_AND_INSTALL_ARGS -DGLM_TEST_ENABLE=OFF
#)

# GLEW
if (NOT APPLE)
    include(AutoBuildGLEW)
    AutoBuild_use_package_GLEW(${PROJECT_NAME} PUBLIC)
endif()

#Assimp asset import library
include(AutoBuildAssimp)
AutoBuild_use_package_Assimp(${PROJECT_NAME} PUBLIC)

#zlib is included by assimp.
if (ZLIB_FOUND)
  target_link_libraries(${PROJECT_NAME} PUBLIC ZLIB::ZLIB)
endif()

#SOIL
include(AutoBuildSOIL)
AutoBuild_use_package_SOIL(${PROJECT_NAME} PUBLIC)


target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DEST}>        # for client in install mode
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/glm>  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DEST}/glm>        # for client in install mode
)



install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets COMPONENT CoreLib
  LIBRARY DESTINATION ${INSTALL_LIB_DEST}
  ARCHIVE DESTINATION ${INSTALL_LIB_DEST}
  RUNTIME DESTINATION ${INSTALL_BIN_DEST}
)

install(FILES ${header_files} DESTINATION ${INSTALL_INCLUDE_DEST} COMPONENT CoreLib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/glm DESTINATION ${INSTALL_INCLUDE_DEST} COMPONENT CoreLib)
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/glad/include/KHR DESTINATION ${INSTALL_INCLUDE_DEST} COMPONENT CoreLib)

if (WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "./Debug")
	#set_target_properties(${WINDOWS_BINARIES} PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	add_definitions( -DNOMINMAX )
endif(WIN32)


## ************************************************ CREATE A CMAKE CONFIG FILE TO HELP OTHER PROJECTS

message("WRITING CMAKE CONFIG PACKAGE")

# The documentation for setting this up is so bad.  These pages all helped.  My current
# understanding (dfk) is summarized below to help others.
# https://cmake.org/cmake/help/git-master/manual/cmake-packages.7.html#creating-packages
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements
# http://foonathan.net/blog/2016/03/03/cmake-install.html
# https://archive.fosdem.org/2013/schedule/event/moderncmake/attachments/slides/258/export/events/attachments/moderncmake/slides/258/cmake_fosdem_2013.pdf

# CMake Package Config in a Nutshell:

# CMake config packages are the newer, recommended way of distributing projects built with CMake.
# They replace the older FindModule.cmake approach for including dependencies in a cmake project, 
# but not everyone has yet switched over to the new approach.  Both styles can work together, and
# this seems to often be required because many libraries are not yet distributing a *Config.cmake
# file with their install tree.


# 1. The Difference Between "Find Modules" (Find*.cmake) and "Config Packages" (*Config.cmake)

# Whereas, Find*.cmake files are either installed on the system or written by downstream projects.
# *Config.cmake files are created and installed by the upstream projects (e.g., we want generate
# MinVRConfig.cmake and install it whenever the MinVR project in installed).

# When you use a Find Module, it defines several variables (e.g., OPENGL_INCLUDE_DIR, OPENGL_LIBRARIES)
# that you must then be sure to add to the include dirs and libs to link with your project.
# Config Packages are different.  Instead, they "export" the targets defined in the original project
# (e.g., the target MinVR is used in this file to build libMinVR.a) so that the client project
# may refer to these targets directly.  This is pretty cool because we can define the MinVR target
# in such a way that cmake even knows which header files are associated with it, and will add the
# required include flags whenever the MinVR target is used.


# 2. How to use Config Packages in a client CMakeLists.txt 

# With the config package appraoch, client projects only need to include two lines to use our library:

# find_package(MinVR REQUIRED)
# ... then, after add_executable(${PROJECT_NAME}) or add_library(${PROJECT_NAME})
# target_link_libraries(${PROJECT_NAME} PUBLIC MinVR)


# 3. Finding *Config.cmake files

# Config packages are files of the form MyProjNameConfig.cmake (or *-config.cmake).  Each library
# provides and installs its own config package.  So, here we will create one to install with MinVR.
# If created correctly, these packages can be relocatable, i.e., not specific to the install path
# used on a particular machine.  When you call find_package(...) cmake looks for packages in a
# variety of locations (e.g., /usr/local/lib/cmake/*).  

# Since config packages are supposed to be written by and installed by the upstream dependencies,
# my understanding is that you should not attempt to write you own version of a config package if
# you encounter a project that is not (yet) using the config package approach.  Instead, use the
# older Find Module approach.  If CMake ships with a Find*.cmake file for that project, then use 
# it.  If not, then you can write your own.  Since making this switch, many of the newer
# Find*.cmake scripts that ship with CMake have been updated to support imported targets. They
# are still called something like FindOpenGL.cmake, but in addition to just setting vars like
# OPENGL_INCLUDE_DIR, they also define targets, such as OpenGL::GL.  We use these when we can
# because they are more robust.  When you do target_link_libraries(myproj PULBIC OpenGL::GL)
# cmake knows to not only link with the the opengl library but also to include its headers and
# link with all dependencies.


# This tells cmake to generate a MinVRTargets.cmake file that works for importing all targets
# associated with the MinVR export group from the install tree.  This file is where all the
# important cmake magic happens. It is auto-generated.  This call also installs the file to 
# the appropriate location.
install(EXPORT BasicGraphicsTargets
  NAMESPACE BasicGraphics::
  FILE BasicGraphicsTargets.cmake
  DESTINATION ${INSTALL_CMAKECONFIG_DEST}
)

# This is a very small wrapper around the MinVRTargets.cmake file.  It's possible to use this
# configure_file() command to replace variables (e.g., special defines, install paths) in the 
# file, but we do not currently need that functionality.
configure_file(
  cmake/BasicGraphicsConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/BasicGraphicsConfig.cmake"
  COPYONLY
)

# This auto-generates a standard CMake config version file. Using this, client projects can
# check to see if a specific version of MinVR is installed on the system.
include(CMakePackageConfigHelpers)
set_property(TARGET BasicGraphics PROPERTY VERSION 1.0)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/BasicGraphicsConfigVersion.cmake"
  VERSION 1.0
  COMPATIBILITY AnyNewerVersion
)

# The two Config files also get installed
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/BasicGraphicsConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/BasicGraphicsConfigVersion.cmake"
  DESTINATION
    ${INSTALL_CMAKECONFIG_DEST}
)

AutoBuild_check_status()					 