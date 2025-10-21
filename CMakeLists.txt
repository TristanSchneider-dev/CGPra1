# Copyright (C) 2013, 2014, 2025
# Computer Graphics Group, University of Siegen
# Written by Martin Lambers <martin.lambers@uni-siegen.de>
# Modified by Marina Evers <marina.evers@uni-siegen.de>
#
# Copying and distribution of this file, with or without modification, are
# permitted in any medium without royalty provided the copyright notice and this
# notice are preserved. This file is offered as-is, without any warranty.

cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0017 NEW)
cmake_policy(SET CMP0020 NEW)

project("GraPra 2025 - Tycho Brahe" C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

#if(CMAKE_COMPILER_IS_GNUCXX)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Wextra")
#endif()

# Required libraries
find_package(Qt5OpenGL 5.4.0 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# process .ui files
############################
# List your ui files here  #
# (if you need any)        #
############################
qt5_wrap_ui(ui_mainwindow.h gui/mainwindow.ui)

#process resource files
############################
# List your qrc files here #
# (if you need any)        #
############################
qt5_add_resources (ResourceSources images.qrc shader.qrc)

# Optional libraries
find_package(GTA QUIET)

# The utility library
add_subdirectory(glbase)

# The program
################################
# List your project files here #
################################
add_executable(tychobrahe
    main.cpp
    ${ResourceSources}
    image/image.cpp
    image/image.h
    gui/mainwindow.ui
    gui/mainwindow.cpp
    gui/glwidget.cpp
    gui/glwidget.hpp
    gui/config.cpp
    gui/config.h
    planets/cone.cpp
    planets/cone.h
    planets/coordinatesystem.cpp
    planets/coordinatesystem.h
    planets/deathstar.cpp
    planets/deathstar.h
    planets/drawable.cpp
    planets/drawable.h
    planets/orbit.cpp
    planets/orbit.h
    planets/path.cpp
    planets/path.h
    planets/planet.cpp
    planets/planet.h
    planets/skybox.cpp
    planets/skybox.h
    planets/sun.cpp
    planets/sun.h

    shader/cube.vs.glsl
    shader/cube.fs.glsl
)

include_directories(${CMAKE_SOURCE_DIR}/glbase ${OPENGL_INCLUDE_DIR})

####################################
# Build with correct opengl lib    #
# (this could fail on your system) #
####################################
if(WIN32 OR CYGWIN)
        target_link_libraries(tychobrahe opengl32 libglbase Qt5::OpenGL ${OPENGL_gl_LIBRARY})
elseif(APPLE)
        target_link_libraries(tychobrahe libglbase Qt5::OpenGL ${OPENGL_gl_LIBRARY})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenGL")
else()
        target_link_libraries(tychobrahe GL libglbase Qt5::OpenGL ${OPENGL_gl_LIBRARY})
endif()

if(GTA_FOUND)
        add_definitions(-DHAVE_GTA)
        include_directories(${GTA_INCLUDE_DIR})
        target_link_libraries(tychobrahe ${GTA_LIBRARIES})
endif()

# copy images
add_custom_command(TARGET tychobrahe POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/images $<TARGET_FILE_DIR:tychobrahe>/images)

install(TARGETS tychobrahe RUNTIME DESTINATION bin)
