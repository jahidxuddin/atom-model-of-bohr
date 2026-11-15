cmake_minimum_required(VERSION 3.25)
project(atom_model_of_bohr C)

set(CMAKE_C_STANDARD 11)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        cglm
        GIT_REPOSITORY https://github.com/recp/cglm.git
        GIT_TAG v0.9.4
)
FetchContent_MakeAvailable(cglm)

find_package(OpenGL REQUIRED)

add_executable(atom_model_of_bohr
        src/main.c
        src/shader.c
        external/glad/src/glad.c
        src/engine.c
        include/engine.h
        src/primitives.c
        include/primitives.h
        src/atom.c
        include/atom.h
)

target_include_directories(atom_model_of_bohr PRIVATE
        include
        external/glad/include
)

target_link_libraries(atom_model_of_bohr PRIVATE glfw OpenGL::GL cglm)