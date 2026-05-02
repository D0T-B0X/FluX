# FluX

FluX is a real-time smoothed particle hydrodynamics (SPH) fluid simulation built with OpenGL 4.6 compute shaders. The simulation runs entirely on the GPU: particles are spatially hashed and sorted, SPH densities and forces are computed, and the results are rendered as instanced spheres in a GLFW window.

This project is focused on the compute and rendering pipeline rather than a full application framework. It is a compact, GPU-first implementation that you can read and extend.

## What it does
- Initializes a cubic particle volume based on `GRID_SIDE` in `include/settings.h`.
- Advances physics using a fixed timestep (`PHYSICS_DT`) while the render loop runs independently.
- Computes neighbor access using spatial hashing and a radix-sort pipeline on the GPU.
- Runs SPH density, pressure, and force passes in compute shaders.
- Renders each particle as an instanced sphere with basic lighting.

## How the simulation is organized
- `Scene` owns the particle data and the shader storage buffers (SSBOs).
- `Physics` manages the compute shaders and the dispatch pipeline:
	- Hash grid cell indices.
	- Radix-sort particle indices (prefix scan, global offset sum, scatter).
	- Compute SPH density, pressure, and forces.
- `Renderer` manages the window, camera input, and draws instanced spheres each frame.

Shader sources are in `shaders/Compute` and `shaders/Render`. The build generates a `config.h` that bakes absolute shader paths, so a reconfigure is needed if you move the repository.

## Controls
- Mouse: look around (cursor is captured)
- W/A/S/D: move camera
- Space: move up
- Left Ctrl: move down
- Esc: close window

## Requirements
- C++20 compiler
- CMake 4.1+
- OpenGL 4.6 capable GPU/driver (GLSL 4.60)
- GLFW3 development package and pkg-config

GLAD and GLM are included in this repository, so you do not need to install them separately.

## Build
```bash
cmake -S . -B build
cmake --build build -j
```

## Run
```bash
./build/FluX
```

Note: shader paths are baked into build-time configuration. If you move the repo or shaders, re-run CMake so the generated config stays valid.

## Configuration
Simulation constants (grid size, time step, SPH constants, gravity, viscosity) are defined in `include/settings.h`. Adjust these to trade off quality vs performance.

## Project layout
- `include/`: public headers (scene, renderer, physics, settings)
- `src/`: engine implementation and entry point
- `shaders/`: compute and render shaders
- `build/`: out-of-source build artifacts (generated)

## Notes and limitations
- The radix-sort global offset phase is designed around a fixed maximum of workgroups; very large particle counts may require adjustments in the compute pipeline.
- The physics loop prints per-frame compute timings and an average once the app exits.

## License
MIT. See [LICENSE](LICENSE).
