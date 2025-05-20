cmake_minimum_required(VERSION 3.20)

project(hello)
find_package(SDL3)

file(GLOB SOURCES "src/*.c")
add_executable(app ${SOURCES}) # WIN32 - hide the console like this add_executable(app WIN32)

target_link_libraries(app PRIVATE SDL3::SDL3)
