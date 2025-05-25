cmake_minimum_required(VERSION 3.20)
project(SDL3_Project)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

if(NOT DEFINED ENV{SDL3_PATH})
    message(FATAL_ERROR "Please set SDL3_PATH environment variable")
endif()

set(SDL3_PATH $ENV{SDL3_PATH})

include_directories("${SDL3_PATH}/include")
link_directories("${SDL3_PATH}/lib")

# on Web targets, we need CMake to generate a HTML webpage. 
# if(EMSCRIPTEN)
#   set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
# endif()

# Gather source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(app WIN32 ${SOURCES}) # WIN32 hides the console

# Link SDL3 dynamically
target_link_libraries(app PRIVATE SDL3)

# Place built executable in bin subdirectory
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

file(COPY "${SDL3_PATH}/bin/SDL3.dll"
     DESTINATION "${CMAKE_BINARY_DIR}/bin")
