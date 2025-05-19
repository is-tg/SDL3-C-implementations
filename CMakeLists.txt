cmake_minimum_required(VERSION 3.20)

project(solar-sim)
add_executable(app) # WIN32 - hide the console like this add_executable(app WIN32)

file(GLOB SOURCES "src/*.c")
target_sources(app PRIVATE ${SOURCES})

find_package(SDL3)
target_link_libraries(app PRIVATE SDL3::SDL3)
target_link_options(app PRIVATE -static)
