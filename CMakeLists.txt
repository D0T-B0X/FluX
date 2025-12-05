cmake_minimum_required(VERSION 4.1)
project(FluX)

find_package(PkgConfig REQUIRED)
find_package(glfw3 REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

set(SHADER_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SURFACE_VPATH "${SHADER_DIR}/vSurface.glsl")
set(SURFACE_FPATH "${SHADER_DIR}/fSurface.glsl")
set(SPHERE_VPATH "${SHADER_DIR}/vSphere.glsl")
set(SPHERE_FPATH "${SHADER_DIR}/fSphere.glsl")
set(MESH_SRC_DIR "${CMAKE_SOURCE_DIR}/src/Mesh")
set(RENDERER_SRC_DIR "${CMAKE_SOURCE_DIR}/src/Renderer")
set(PHYSICS_SRC_DIR "${CMAKE_SOURCE_DIR}/src/Physics")
set(UTIL_SRC_DIR "${CMAKE_SOURCE_DIR}/src/util")

configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${MESH_SRC_DIR}/Sphere3D.cpp
    ${RENDERER_SRC_DIR}/renderer.cpp
    ${RENDERER_SRC_DIR}/camera.cpp
    ${RENDERER_SRC_DIR}/shader.cpp
    ${RENDERER_SRC_DIR}/test.cpp
    ${PHYSICS_SRC_DIR}/physics.cpp
    ${UTIL_SRC_DIR}/eventHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/application.cpp
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRECTORIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES} dl)

target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CFLAGS_OTHER})