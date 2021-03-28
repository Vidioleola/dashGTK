# Set the name and the supported language of the project
PROJECT(DASH_DYN_LIB C)
# Set the minimum version of cmake required to build this project
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# Use the package PkgConfig to detect GTK+ headers/library files
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

SET(GCC_COVERAGE_COMPILE_FLAGS "-rdynamic")
SET(GCC_COVERAGE_LINK_FLAGS    "-lm")

# Add other flags to the compiler
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER} ${GCC_COVERAGE_COMPILE_FLAGS})
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")



# Add an executable compiled from hello.c
ADD_EXECUTABLE(test main.c gisagauge.c)
ADD_LIBRARY(dash SHARED
dash.c
gisagauge.c
)

set_target_properties(dash PROPERTIES SOVERSION 1)
set_target_properties(dash PROPERTIES PUBLIC_HEADER foo.h)

# Link the target to the GTK+ libraries
TARGET_LINK_LIBRARIES(test ${GTK3_LIBRARIES} pthread )
TARGET_LINK_LIBRARIES(dash ${GTK3_LIBRARIES} pthread )
