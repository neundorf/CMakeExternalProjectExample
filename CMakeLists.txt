cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(someproject)

# Setup the build types of this project
set(CMAKE_CONFIGURATION_TYPES Release;Debug;RelWithDebInfo)

# Set the default build type to Release, both for single- and multi-config generators
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(isMultiConfig)
   set(CMAKE_DEFAULT_BUILD_TYPE Release)
elseif(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

######### set up an external project

include(ExternalProject)

# For building the external project we'll use ninja.
# Because
#  - it can build parallel
#  - it is available as a package under Linux
#  - it is installed along with Visual Studio
find_program(NINJA_EXECUTABLE ninja)
if(NOT NINJA_EXECUTABLE)
   message(FATAL_ERROR "Did not find the ninja build tool !")
endif()

# Set a base directory for the external project
set_property(DIRECTORY PROPERTY EP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ExternalProjects )

# The external project foo will install its files here:
set(INST_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/Install)

# Set up building libfoo via externalproject.
# externalproject_add() supports generator expressions, so we can forward the current build type
# via $<CONFIG> to a single-config build of the external project.
externalproject_add(EP_libfoo
       SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libfoo
       INSTALL_DIR ${INST_BASE_DIR}
       CMAKE_ARGS --no-warn-unused-cli -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>/$<CONFIG> -DCMAKE_BUILD_TYPE=$<CONFIG> -G Ninja
       BUILD_BYPRODUCTS ${INST_BASE_DIR}/$<CONFIG>/lib/libfoo.so 
       DOWNLOAD_COMMAND "" )

# Create the imported library
add_library(FOO::FOO SHARED IMPORTED)
set_property(TARGET FOO::FOO PROPERTY IMPORTED_CONFIGURATIONS Release;RelWithDebInfo;Debug)

# Set up the necessary properties for a shared library.
# This is some manual work and somewhat fragile, but it works.
if(UNIX)
   set_property(TARGET FOO::FOO PROPERTY IMPORTED_LOCATION_RELEASE        ${INST_BASE_DIR}/Release/lib/libfoo.so)
   set_property(TARGET FOO::FOO PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${INST_BASE_DIR}/RelWithDebInfo/lib/libfoo.so)
   set_property(TARGET FOO::FOO PROPERTY IMPORTED_LOCATION_DEBUG          ${INST_BASE_DIR}/Debug/lib/libfoo.so)
else()
   message(FATAL_ERROR "Windows settings not yet added here.")
endif()

# Set the include directories for the imported target per build configuration.
# We have to create these directories in advance, if they are only created when
# the external project is built, it doesn't work, since cmake checks that they exist
# already at cmake time.
target_include_directories(FOO::FOO INTERFACE ${INST_BASE_DIR}/$<CONFIG>/include)
file(MAKE_DIRECTORY ${INST_BASE_DIR}/Debug/include )
file(MAKE_DIRECTORY ${INST_BASE_DIR}/Release/include )
file(MAKE_DIRECTORY ${INST_BASE_DIR}/RelWithDebInfo/include )

# Make the imported library depend on the external project, so building an executable
# which links against the imported target triggers building the external project.
add_dependencies(FOO::FOO EP_libfoo)

######### end of setting up the external project

# Create an executable which links agains the imported target FOO::FOO which
# is built by the external project.
add_executable(footest main.cpp)
target_link_libraries(footest  FOO::FOO)
