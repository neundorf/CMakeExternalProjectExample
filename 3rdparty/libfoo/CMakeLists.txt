cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(fooproj)

add_library(foo SHARED foo.cpp)
target_compile_definitions(foo PRIVATE CFG=\"$<CONFIG>\" )

install(TARGETS foo DESTINATION lib)
install(FILES foo.h DESTINATION include/foo)
