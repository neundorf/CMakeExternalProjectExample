cmake_minimum_required(VERSION 3.21)

project(someproject)

include(ExternalProject)

set(CMAKE_CONFIGURATION_TYPES Release;Debug;RelWithDebInfo)

set(INST_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/Install)

set(CMAKE_DEFAULT_BUILD_TYPE Release)

set_property(DIRECTORY PROPERTY EP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ExternalProjects )

externalproject_add(EP_libfoo
       SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libfoo
       INSTALL_DIR ${INST_BASE_DIR}
       CMAKE_ARGS --no-warn-unused-cli -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>/$<CONFIG> -DCMAKE_BUILD_TYPE=$<CONFIG> -G Ninja
       BUILD_BYPRODUCTS ${INST_BASE_DIR}/$<CONFIG>/lib/libfoo.so 
       DOWNLOAD_COMMAND "" )

add_library(FOO::FOO SHARED IMPORTED)
set_property(TARGET FOO::FOO PROPERTY IMPORTED_CONFIGURATIONS Release;RelWithDebInfo;Debug)
set_property(TARGET FOO::FOO PROPERTY IMPORTED_LOCATION_RELEASE        ${INST_BASE_DIR}/Release/lib/libfoo.so)
set_property(TARGET FOO::FOO PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${INST_BASE_DIR}/RelWithDebInfo/lib/libfoo.so)
set_property(TARGET FOO::FOO PROPERTY IMPORTED_LOCATION_DEBUG          ${INST_BASE_DIR}/Debug/lib/libfoo.so)

file(MAKE_DIRECTORY ${INST_BASE_DIR}/Debug/include )
file(MAKE_DIRECTORY ${INST_BASE_DIR}/Release/include )
file(MAKE_DIRECTORY ${INST_BASE_DIR}/RelWithDebInfo/include )
target_include_directories(FOO::FOO INTERFACE  ${INST_BASE_DIR}/$<CONFIG>/include)

add_dependencies(FOO::FOO EP_libfoo)

add_executable(footest main.cpp)
target_link_libraries(footest  FOO::FOO)
