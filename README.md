# FluX

FluX is a GPU-based fluid simulation prototype written in C++ and OpenGL 4.6. It combines SPH-inspired compute passes with instanced sphere rendering for real-time visualization.

## Highlights

- Modern OpenGL 4.6 pipeline using GLFW and GLAD
- Compute-shader simulation passes for density, pressure, and force/integration
- Uniform-grid cell hashing and radix-sort prefix-scan stages
- Instanced rendering of particle spheres
- Fixed-step simulation and rendering loops
- Free-fly camera controls for scene inspection

## Architecture Overview

FluX is organized into three primary runtime components:

- Application layer: Initializes scene state, configures simulation parameters, and runs fixed-step loops.
- Physics engine: Owns compute shaders, uploads SSBOs, computes cell hashes, and executes SPH passes.
- Renderer: Creates window/context, uploads sphere mesh data, and draws particles each render tick.

At startup, the app generates an initial particle block inside a bounded volume and uploads particle attributes to shader storage buffer objects (SSBOs).

## Simulation Pipeline

Each physics update executes the following high-level flow:

1. Grid build pass
2. Radix-sort support passes (count, local scan, block-sum scan, combine)
3. SPH density pass
4. SPH pressure pass
5. SPH force/integration pass

Particle attributes are stored in SSBOs:

- position_mass
- velocity_density
- force_pressure
- color_padding
- cell_index
- count/block/local/offset buffers for sort support

Pressure uses a Tait-style equation of state.

## Controls

- W / S: Move forward and backward
- A / D: Strafe left and right
- Space / Left Ctrl: Move up and down
- Mouse: Look around
- Esc: Exit

## Requirements

- Linux
- OpenGL 4.6-capable GPU and driver
- C++20 compiler
- CMake 4.1+
- GLFW3
- pkg-config

## Build and Run

```bash
cmake -S . -B build
cmake --build build -j"$(nproc)"
./build/FluX
```

## Configuration

Primary runtime constants are defined in include/settings.h.

Current defaults:

- Window: 1920 x 1200
- Grid side length: 16
- Initial particle count: 16^3 = 4096
- Physics timestep: 0.0020 s
- Render timestep: 0.0166 s
- Rest density: 1000.0 kg/m^3
- Gravity: (0.0, -9.81, 0.0)
- Compute local size: 256 threads

Shader source paths are generated at configure time through config.h.

## Current Status

Implemented:

- Core simulation and rendering loop
- SPH density/pressure/force passes
- Particle floor collision response
- Cell hash generation
- Radix-sort support stage wiring
- Instanced sphere rendering

In progress or partial:

- End-to-end sorted-neighbor usage in SPH kernels
- Surface rendering path
- Event-driven runtime parameter updates

## Limitations

- SPH compute shaders currently perform full particle-pair loops, which are expensive at larger particle counts.
- Event handling scaffolding exists but is not yet integrated as a complete runtime system.
- Surface generation and rendering are present as scaffolding and are not part of the active rendering flow.

## Repository Layout

```text
FluX/
  include/
    Physics/
    Renderer/
    Mesh/
    Object/
    util/
    application.h
    scene.h
    settings.h
  src/
    Physics/
    Renderer/
    Mesh/
    util/
    application.cpp
    scene.cpp
    main.cpp
    glad.c
  shaders/
    SPH/
      density_pass.comp
      pressure_pass.comp
      force_pass.comp
    SpatialHashing/
      hash_grid_cell_index.comp
      RadixSort/
        count.comp
        local_scan.comp
        block_sum_scan.comp
        combine.comp
    Render/
      vSphere.glsl
      fSphere.glsl
      vSurface.glsl
      fSurface.glsl
  CMakeLists.txt
  config.h.in
```

## License

MIT