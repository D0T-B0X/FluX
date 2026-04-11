# FluX

FluX is a real-time fluid simulation project built with C++, OpenGL 4.6, and GLSL compute shaders. It uses an SPH-style particle model on the GPU and renders particles via instanced sphere drawing.

## Overview

The application runs two fixed-step loops:

- Physics update at 2 ms per step
- Render update at 16.6 ms per step

At startup, FluX initializes a dense particle block inside a bounded volume, uploads particle properties to SSBOs, runs compute passes each physics tick, and renders all particles as sphere instances.

## Current Implementation Status

Implemented and active:

- OpenGL 4.6 renderer with GLFW + GLAD
- GPU SPH passes:
   - Density pass (Poly6 kernel)
   - Pressure pass (Tait equation)
   - Force/integration pass (pressure, viscosity, gravity, floor collision damping)
- GPU cell index hashing pass for spatial partitioning
- Radix-sort support shaders wired into the frame pipeline (count + local scan stages)
- Instanced sphere rendering with configurable mesh subdivision
- Free-fly camera controls

In progress:

- Complete and validate full radix-sort pipeline for neighbor-friendly ordering
- Integrate sorted/grid data into neighbor queries in SPH passes
- Surface rendering path and event-driven runtime parameter edits

## Simulation Notes

SPH pressure uses the Tait equation:

$$
p = k\left(\left(\frac{\rho}{\rho_0}\right)^\gamma - 1\right)
$$

Particle attributes are stored in SSBOs:

- position_mass
- velocity_density
- force_pressure
- color_padding
- cell_indices
- count/offset buffers for sorting stages

## Controls

- W / S: move forward / backward
- A / D: strafe left / right
- Space / Left Ctrl: move up / down
- Mouse: look around
- Esc: close application

## Build Requirements

- Linux
- OpenGL 4.6 capable GPU/driver
- C++20 compiler
- CMake 4.1 or newer
- GLFW3
- pkg-config

## Build and Run

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j"$(nproc)"
./FluX
```

## Key Configuration

Primary simulation and runtime constants are defined in include/settings.h.

Current defaults include:

- Window size: 1920 x 1200
- Grid side length: 47
- Particle count at startup: 47^3 = 103,823
- Physics step: 0.002 s
- Render step: 0.0166 s
- Rest density: 1000 kg/m^3
- Gravity: (0, -9.81, 0)
- Compute workgroup size: 256 threads

## Project Structure

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
      Render/
         vSphere.glsl
         fSphere.glsl
         vSurface.glsl
         fSurface.glsl
   CMakeLists.txt
   config.h.in
```

## License

MIT License