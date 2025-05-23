cmake_minimum_required(VERSION 3.20)
project(SDL3_Project)

# Manually set paths since we don't have SDL3Config.cmake
include_directories("C:/Users/Tharun/Documents/SDL3-x64-2025-05-18/SDL3-x64/include")
link_directories("C:/Users/Tharun/Documents/SDL3-x64-2025-05-18/SDL3-x64/lib")

# Gather source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(app ${SOURCES}) # WIN32 hides the console

# Link SDL3 dynamically
target_link_libraries(app PRIVATE SDL3)

# Ensure the SDL3.dll is available at runtime
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

