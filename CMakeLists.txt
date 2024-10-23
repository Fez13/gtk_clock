set(CMAKE_C_COMPILER "/usr/bin/x86_64-w64-mingw32-gcc")  # Set to the MinGW-w64 GCC compiler
set(CMAKE_CXX_COMPILER "/usr/bin/x86_64-w64-mingw32-g++")  # Set to the MinGW-w64 G++ compiler

cmake_minimum_required(VERSION 3.22.1)
project(gtk_clock)
set(CMAKE_CXX_STANDARD 23)
include(ExternalProject)

set(CMAKE_CXX_FLAGS_DEBUG "-g  -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-m64 -O3 -march=native -DNDEBUG")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-narrowing")

set(BUILD_DIR_WINDOWS "${CMAKE_BINARY_DIR}/windows")
set(BUILD_DIR_LINUX "${CMAKE_BINARY_DIR}/linux")
option(WINDOWS "Windows Build" OFF)

if(WINDOWS)
  # Set up the environment for cross-compiling
  set(CMAKE_SYSTEM_NAME Windows)

  # Ensure that pkg-config uses the MinGW-w64 paths (replace with actual paths)
  set(PKG_CONFIG_EXECUTABLE "/usr/bin/x86_64-w64-mingw32-pkg-config")

  # Find the required packages using pkg-config for MinGW-w64
  pkg_check_modules(GTK REQUIRED gtkmm-3.0)
  pkg_check_modules(GTK_LAYER_SHELL REQUIRED gtk-layer-shell-0)
  pkg_check_modules(LIBRSVG REQUIRED librsvg-2.0)
  pkg_check_modules(FREETYPE REQUIRED freetype2)


    # Point to the MinGW-w64 toolchain's root
    set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32")

    # Ensure CMake searches for libraries and includes within the MinGW-w64 root
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

else()
  # Native Linux build
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK REQUIRED gtkmm-3.0)
  pkg_check_modules(GTK_LAYER_SHELL REQUIRED gtk-layer-shell-0)
  pkg_check_modules(LIBRSVG REQUIRED librsvg-2.0)
  pkg_check_modules(FREETYPE REQUIRED freetype2)
endif()

# Include the necessary directories
include_directories(${GTK_INCLUDE_DIRS})
include_directories(${GTK_LAYER_SHELL_INCLUDE_DIRS})
include_directories(${LIBRSVG_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})

# Add linker flags for GTK, GTK Layer Shell, and librsvg
link_directories(${GTK_LIBRARY_DIRS})
link_directories(${GTK_LAYER_SHELL_LIBRARY_DIRS})
link_directories(${LIBRSVG_LIBRARY_DIRS})
link_directories(${FREETYPE_LIBRARY_DIRS})

# Set compiler flags
add_definitions(${GTK_CFLAGS_OTHER})
add_definitions(${GTK_LAYER_SHELL_CFLAGS_OTHER})
add_definitions(${LIBRSVG_CFLAGS_OTHER})

file(GLOB_RECURSE H_FILES "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE CPP_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB HPP_JSON "include/json/json.hpp")

if(WINDOWS)
  add_compile_definitions(WINDOWS_BUILD)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR_WINDOWS})
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR_LINUX})
endif()

add_executable(${PROJECT_NAME} main.cpp ${H_FILES} ${CPP_FILES}  ${HPP_JSON})

# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK_LIBRARIES} stdc++
                      ${GTK_LAYER_SHELL_LIBRARIES} ${LIBRSVG_LIBRARIES} ${FREETYPE_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# Specify where to install the binary and config file
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
