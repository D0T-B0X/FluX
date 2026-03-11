# FluX

FluX is a real-time 3D particle-based fluid simulator built from scratch with C++ and OpenGL 4.6. It uses **Smoothed Particle Hydrodynamics (SPH)** to simulate fluid behavior entirely on the GPU via compute shaders.

---

## Features

- **GPU-driven SPH simulation** — density, pressure, and force computations each run in a dedicated GLSL 460 compute shader (3 passes per frame) with 256-thread workgroups
- **4,096 particles** initialized on a 16×16×16 grid with position-based color gradients
- **Instanced rendering** — all particles share a single subdivided cube-sphere mesh drawn via `glDrawElementsInstanced`, with per-particle data streamed from an SSBO
- **Phong lighting** — per-fragment diffuse + ambient shading on every particle
- **Free-look camera** — WASD + mouse first-person controls with configurable FOV, sensitivity, and speed
- **Floor boundary** — particles bounce off a configurable floor plane with velocity damping
- **Event system** — priority-queue based handler for runtime parameter changes (radius, subdivisions, viscosity, gravity)
- **Tait equation of state** for pressure, with Poly6, Spiky gradient, and viscosity Laplacian SPH kernels

## Project Structure

```
FluX/
├── src/
│   ├── main.cpp                 # Entry point
│   ├── application.cpp          # Main loop & scene setup
│   ├── scene.cpp                # Scene state & particle storage
│   ├── Mesh/
│   │   ├── Sphere3D.cpp         # Cube-sphere mesh generator
│   │   └── Surface3D.cpp        # Grid surface generator
│   ├── Renderer/
│   │   ├── renderer.cpp         # OpenGL rendering pipeline
│   │   ├── camera.cpp           # First-person camera
│   │   └── shader.cpp           # Shader compilation & uniforms
│   ├── Physics/
│   │   └── physics.cpp          # Compute shader dispatch & SSBO management
│   └── util/
│       └── eventHandler.cpp     # Priority-queue event system
├── shaders/
│   ├── vSphere.glsl             # Sphere vertex shader (instanced)
│   ├── fSphere.glsl             # Sphere fragment shader (Phong)
│   ├── vSurface.glsl            # Surface vertex shader
│   ├── fSurface.glsl            # Surface fragment shader
│   ├── density_pass.comp        # SPH density compute shader (Poly6 kernel)
│   ├── pressure_pass.comp       # Pressure compute shader (Tait equation)
│   └── force_pass.comp          # Force, integration & boundary compute shader
├── include/                     # Headers mirroring src/ layout
├── CMakeLists.txt
└── config.h.in                  # CMake-configured shader paths
```

## Building

### Prerequisites

- A C++17 compiler (GCC, Clang)
- CMake 4.1+
- GLFW 3 (via pkg-config)
- OpenGL 4.6 capable GPU and drivers

### Build Steps

```bash
git clone https://github.com/D0T-B0X/FluX.git
cd FluX
mkdir -p build && cd build
cmake ..
make -j$(nproc)
./FluX
```

## Controls

| Key | Action |
|-----|--------|
| `W` / `S` | Move forward / backward |
| `A` / `D` | Strafe left / right |
| `Space` / `Ctrl` | Move up / down |
| `Mouse` | Look around |
| `Esc` | Quit |

## How It Works

1. **Initialization** — 4,096 particles are placed on a uniform 16³ grid. Each particle carries position, mass, velocity, density, force, pressure, and color packed into four `vec4`s (64 bytes per particle). The data is uploaded to an SSBO shared between the compute and render pipelines.

2. **Compute passes** — Every frame, three compute shaders run in sequence with memory barriers between them:
   - **Density pass** — accumulates density for each particle from all neighbors using the **Poly6** kernel
   - **Pressure pass** — computes pressure via **Tait's equation**: $p = k\left(\left(\frac{\rho}{\rho_0}\right)^\gamma - 1\right)$
   - **Force pass** — calculates pressure forces with the **Spiky gradient** kernel and viscosity forces with the **viscosity Laplacian** kernel, integrates velocity and position, and enforces floor boundary conditions

3. **Render pass** — The particle SSBO is bound as instanced vertex attributes. A shared cube-sphere mesh is drawn once per particle via `glDrawElementsInstanced`, with per-fragment Phong lighting (diffuse + ambient).

## Roadmap

- [ ] Spatial hashing / grid-based neighbor search for O(n) density lookups
- [ ] Scale to 10k–100k+ particles
- [ ] Wall boundaries on all sides
- [ ] Surface mesh rendering (marching cubes or screen-space fluids)
- [ ] Runtime parameter tuning via the event system

## License

[MIT](LICENSE)