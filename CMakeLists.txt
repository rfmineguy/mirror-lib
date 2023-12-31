cmake_minimum_required(VERSION 3.20)
project(mirrors LANGUAGES C)
# project(mirrors-editor LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")

include(FetchContent)

FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG 4.5.0)
FetchContent_MakeAvailable(raylib)

add_library(mirrorlib src/mirror_lib.c)
target_link_libraries(mirrorlib PRIVATE raylib)
target_include_directories(mirrorlib PRIVATE ${raylib_INCLUDE_DIR})

if (BUILD_EXECUTABLE)
add_executable(mirrors src/main.c)
target_link_libraries(mirrors PRIVATE raylib mirrorlib)
target_include_directories(mirrors PRIVATE ${raylib_INCLUDE_DIR} ${rimgui_INCLUDE_DIR})
endif ()
