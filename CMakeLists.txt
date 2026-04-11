cmake_minimum_required(VERSION 4.1)
project(FluX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
find_package(glfw3 REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

set(SHADER_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SURFACE_VPATH "${SHADER_DIR}/Render/vSurface.glsl")
set(SURFACE_FPATH "${SHADER_DIR}/Render/fSurface.glsl")
set(SPHERE_VPATH "${SHADER_DIR}/Render/vSphere.glsl")
set(SPHERE_FPATH "${SHADER_DIR}/Render/fSphere.glsl")
set(GRID_CELL_CPATH "${SHADER_DIR}/SpatialHashing/hash_grid_cell_index.comp")
set(COUNT_CPATH "${SHADER_DIR}/SpatialHashing/RadixSort/count.comp")
set(LOCAL_SCAN_CPATH "${SHADER_DIR}/SpatialHashing/RadixSort/local_scan.comp")
set(DENSITY_CPATH "${SHADER_DIR}/SPH/density_pass.comp")
set(PRESSURE_CPATH "${SHADER_DIR}/SPH/pressure_pass.comp")
set(FORCE_CPATH "${SHADER_DIR}/SPH/force_pass.comp")
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
    ${CMAKE_SOURCE_DIR}/src/scene.cpp
    ${CMAKE_SOURCE_DIR}/src/application.cpp
    ${MESH_SRC_DIR}/Sphere3D.cpp
    ${MESH_SRC_DIR}/Surface3D.cpp
    ${RENDERER_SRC_DIR}/renderer.cpp
    ${RENDERER_SRC_DIR}/camera.cpp
    ${RENDERER_SRC_DIR}/shader.cpp
    ${PHYSICS_SRC_DIR}/physics.cpp
    ${UTIL_SRC_DIR}/eventHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRECTORIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES} dl)

target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CFLAGS_OTHER})