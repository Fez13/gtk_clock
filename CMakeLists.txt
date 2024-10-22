set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

cmake_minimum_required(VERSION 3.22.1)
project(gtk_clock)
set(CMAKE_CXX_STANDARD 23)
include(ExternalProject)

set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-m64 -O3 -march=native -DNDEBUG")

set(BUILD_DIR_WINDOWS "${CMAKE_BINARY_DIR}/windows")
set(BUILD_DIR_LINUX "${CMAKE_BINARY_DIR}/linux")
option(WINDOWS "Windows Build" OFF)

# Find GTK, GTK Layer Shell, Wayland, and librsvg
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtkmm-3.0) # or gtk3 depending on your needs
#pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-cursor wayland-egl)
pkg_check_modules(GTK_LAYER_SHELL REQUIRED gtk-layer-shell-0)
pkg_check_modules(LIBRSVG REQUIRED librsvg-2.0)
pkg_check_modules(FREETYPE REQUIRED freetype2)

# Include the necessary directories
include_directories(${GTK_INCLUDE_DIRS})
#include_directories(${WAYLAND_INCLUDE_DIRS})
include_directories(${GTK_LAYER_SHELL_INCLUDE_DIRS})
include_directories(${LIBRSVG_INCLUDE_DIRS})
include_directories(${FREETYPE})

# Add linker flags for GTK, Wayland, GTK Layer Shell, and librsvg
link_directories(${GTK_LIBRARY_DIRS})
#link_directories(${WAYLAND_LIBRARY_DIRS})
link_directories(${GTK_LAYER_SHELL_LIBRARY_DIRS})
link_directories(${LIBRSVG_LIBRARY_DIRS})
link_directories(${FREETYPE})

# Set compiler flags
add_definitions(${GTK_CFLAGS_OTHER})
#add_definitions(${WAYLAND_CFLAGS_OTHER})
add_definitions(${GTK_LAYER_SHELL_CFLAGS_OTHER})
add_definitions(${LIBRSVG_CFLAGS_OTHER})

file(GLOB_RECURSE H_FILES "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE CPP_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB HPP_JSON "include/json/json.hpp")

if(WINDOWS)
  add_compile_definitions(WINDOWS_BUILD)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/windows)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/linux)
endif()


add_executable(${PROJECT_NAME} main.cpp ${H_FILES} ${CPP_FILES}  ${HPP_JSON})
# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK_LIBRARIES} stdc++
                      ${GTK_LAYER_SHELL_LIBRARIES} ${LIBRSVG_LIBRARIES} ${FREETYPE})
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)


# Specify where to install the binary and config file
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
