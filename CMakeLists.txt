cmake_minimum_required(VERSION 3.24)
project(MySDL3App C)

include(FetchContent)

# Fetch SDL3
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.14
)
FetchContent_MakeAvailable(SDL3)

# Collect all source files in src/
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.c)

# Add your executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(app ${SRC_FILES})

# Enable all warnings
if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Werror)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(app PRIVATE /W4 /WX)
endif()

# Link with SDL3
target_link_libraries(app PRIVATE SDL3::SDL3)

if(WIN32)
    add_custom_command(
        TARGET app POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:app>
        VERBATIM
    )
endif()
