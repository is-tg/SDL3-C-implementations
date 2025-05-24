cmake_minimum_required(VERSION 3.20)
project(SDL3_Project)

if(NOT DEFINED ENV{SDL3_PATH})
    message(FATAL_ERROR "Please set SDL3_PATH environment variable")
endif()

set(SDL3_PATH $ENV{SDL3_PATH})

include_directories("${SDL3_PATH}/include")
link_directories("${SDL3_PATH}/lib")

# Gather source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(app WIN32 ${SOURCES}) # WIN32 hides the console

# Link SDL3 dynamically
target_link_libraries(app PRIVATE SDL3)

# Ensure the SDL3.dll is available at runtime
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

