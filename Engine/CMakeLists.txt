cmake_minimum_required(VERSION 3.10)

project(WovenWorld)

include_directories((${CMAKE_SOURCE_DIR}/dependencies/glfw/include))
include_directories(${CMAKE_SOURCE_DIR}/dependencies)

link_directories(${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2022)

add_executable(Engine src/main.c src/glad.c)

target_link_libraries(Engine
    glfw3
    opengl32
    user32
    gdi32
    shell32
)