# FluX

FluX is a real-time smoothed particle hydrodynamics (SPH) fluid simulation built on OpenGL 4.6 compute shaders. The entire simulation runs on the GPU: particles are spatially hashed, sorted, and updated via compute passes, then rendered as instanced spheres in a GLFW window.

The codebase is intentionally compact and GPU-first, focused on the compute and rendering pipeline rather than a full application framework.

## Highlights
- GPU spatial hashing + radix sort for neighbor lookup
- SPH density, pressure, and force integration in compute shaders
- Instanced sphere rendering with basic lighting
- Fixed physics timestep with independent render loop

## Pipeline overview
1. Hash particle positions into grid cell indices.
2. Radix-sort particles by cell index (prefix scan, global offset sum, scatter).
3. Reorder particle buffers using sorted indices.
4. Compute cell boundaries for neighbor lookups.
5. SPH density pass.
6. SPH pressure pass.
7. SPH force pass and integration.
8. Render particles as instanced spheres.

## Controls
- Mouse: look around (cursor captured)
- W/A/S/D: move camera
- Space: move up
- Left Ctrl: move down
- Esc: close window

## Requirements
- C++20 compiler
- CMake 4.1+
- OpenGL 4.6 capable GPU/driver (GLSL 4.60)
- GLFW3 development package and pkg-config

GLAD and GLM are vendored under [include/](include/) so no separate install is required.

## Build
```bash
cmake -S . -B build
cmake --build build -j
```

## Run
```bash
./build/FluX
```

You can also use the helper scripts:
- [test_build.sh](test_build.sh)
- [test_run.sh](test_run.sh)

## Configuration
Simulation constants live in [include/settings.h](include/settings.h). Key knobs include:
- Grid size and bounds (`GRID_SIDE`, `MIN_BOUND`, `MAX_BOUND`)
- Fixed timestep (`PHYSICS_DT`)
- SPH constants (`SPEED_OF_SOUND`, `RESTING_DENSITY`, `K`, `VISCOSITY`)
- Camera and render settings (`FOV`, `MOVEMENT_SPEED`, `SPHERE_RADIUS`)

The build generates `config.h` with absolute shader paths. If you move the repo or shaders, re-run CMake to regenerate the paths.

## Project layout
- [include/](include/): public headers (scene, renderer, physics, settings)
- [src/](src/): engine implementation and entry point
- [shaders/](shaders/): compute and render shaders
- build/: out-of-source build artifacts (generated)

## Notes and limitations
- The radix sort global offset phase assumes up to 1024 workgroups (512 particles per workgroup), which caps particles at 524,288 without changing the compute pipeline.
- The physics loop prints an average compute time when the app exits.

## License
MIT. See [LICENSE](LICENSE).
