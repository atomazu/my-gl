cmake_minimum_required(VERSION 3.10)
project(mygl C)
set(CMAKE_C_STANDARD 23)

set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/glfw)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_library(stb external/stb/src/stb_image.c)
target_include_directories(stb PUBLIC external/stb/include)

add_library(cglm INTERFACE)
target_include_directories(cglm INTERFACE external/cglm/include)

file(GLOB SRC_FILES "src/*.c")
add_executable(mygl ${SRC_FILES})

set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_BINARY_DIR "${CMAKE_BINARY_DIR}/assets")

add_custom_command(
    TARGET mygl
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${ASSETS_SOURCE_DIR}"
            "${ASSETS_BINARY_DIR}"
    COMMENT "Symlinking assets directory into build directory"
)

target_link_libraries(mygl glfw glad stb cglm)

install(TARGETS mygl DESTINATION bin)
