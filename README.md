# FluX

FluX is a real-time smoothed particle hydrodynamics (SPH) fluid simulation built on OpenGL 4.6 compute shaders. The entire simulation runs on the GPU: particles are spatially hashed, sorted, and updated via compute passes, then rendered as instanced spheres in a GLFW window.

The codebase is intentionally compact and GPU-first, focused on the compute and rendering pipeline rather than a full application framework. No external physics or GPU-sort libraries are used; spatial hashing, the parallel radix sort, and the SPH solver are all implemented from scratch in GLSL.

## Highlights
- GPU spatial hashing + parallel radix sort for neighbor lookup, no CPU-side sorting
- SPH density, pressure, and force integration in compute shaders
- Instanced sphere rendering with basic lighting, sharing GPU buffers directly with the physics pass (no CPU round-trip)
- Fixed physics timestep decoupled from the render loop, with a bounded number of substeps per frame to stay responsive under variable frame times
- Non-blocking GPU timing via double-buffered queries, used to profile and tune substep count empirically rather than by guesswork

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
cmake --build build -j $(nproc)
```

## Run
```bash
./build/FluX
```

You can also use the helper script:
- [run.sh](run.sh)

## Configuration
Simulation constants live in [include/settings.h](include/settings.h). Key knobs include:
- Grid size and bounds (`GRID_SIDE`, `MIN_BOUND`, `MAX_BOUND`)
- Fixed timestep (`PHYSICS_DT`)
- SPH constants (`SPEED_OF_SOUND`, `RESTING_DENSITY`, `K`, `VISCOSITY`, `DAMPING_COEFF`)
- Camera and render settings (`FOV`, `MOVEMENT_SPEED`, `SPHERE_RADIUS`)

`SPEED_OF_SOUND` and `VISCOSITY` are tuned for real-time stability rather than physically accurate water. Real-time weakly-compressible SPH is generally run with an artificial speed of sound well below water's actual ~1480 m/s, since stiffness (and therefore numerical stability) scales with its square. Raising `PHYSICS_DT`, `GRID_SIDE`, or `SPEED_OF_SOUND` without adjusting the others can push the simulation into instability; if particles start exploding or pinning to the domain boundary, that's usually the cause.

The build generates `config.h` with absolute shader paths. If you move the repo or shaders, re-run CMake to regenerate the paths.

## Project layout
- [include/](include/): public headers (scene, renderer, physics, settings)
- [src/](src/): engine implementation and entry point
- [shaders/](shaders/): compute and render shaders
- build/: out-of-source build artifacts (generated)

## Performance notes
- Neighbor search cost is bounded by grid cell occupancy, not just particle count. Uneven particle distribution (piling into a small number of cells) can dominate frame time far more than raw particle count would suggest.
- Physics substep count per frame is bounded (`MAX_PHYSICS_STEPS_PER_FRAME`) to avoid the fixed-timestep accumulator spiraling under load; if the substep count is consistently pinned at that cap, the simulation is running in slow motion relative to real time, not real time.
- Per-step GPU cost is not constant. It grows with how many steps are dispatched in a single batch, so per-dispatch overhead, not raw compute throughput, is generally the limiting factor on integrated GPUs at this particle count.

## Notes and limitations
- The radix sort global offset phase assumes up to 1024 workgroups (512 particles per workgroup), which caps particles at 524,288 without changing the compute pipeline.
- The physics loop reports timing (GPU execution time and steps-per-frame) to help tune `PHYSICS_DT` and the substep cap for a given machine.

## License
MIT. See [LICENSE](LICENSE).