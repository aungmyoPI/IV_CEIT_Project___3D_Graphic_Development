# IV_Project — 3D Graphics Development

A real-time 3D graphics project developed as part of the **BE.CEIT final-year project at Technological University of Mandalay**.

The project is a small playable 3D application built from the ground up using **C++ and OpenGL**. It focuses on real-time rendering, character animation, terrain rendering, third-person camera control, instanced rendering, and interactive gameplay systems.

---

## Features

### 🎨 Real-Time 3D Rendering

- OpenGL 3.3 Core rendering pipeline
- 3D terrain rendering with multiple terrain textures
- Phong-based lighting
- Skybox/environment rendering
- Model loading through Assimp
- Texture loading using `stb_image`
- Depth testing and other standard OpenGL rendering techniques

### 🧍 Player System

- Playable 3D character
- Third-person camera
- Camera-relative movement
- Character rotation based on movement direction
- Sprinting
- Skeletal animation
- Multiple player animation states

### ⚔️ Combat

- Normal attack
- Normal skill
- Ultimate skill
- Animation-driven combat states
- Basic player/enemy interaction

### 🌲 Instanced Rendering

The project also explores GPU instancing for rendering large numbers of repeated objects efficiently.

Examples include:

- Instanced vegetation
- Large numbers of environmental objects
- Repeated meshes rendered using a single draw workflow

### 🖥️ User Interface

- Dear ImGui-based main menu
- Game start interface
- In-game guide/information overlay
- Interactive UI controls

---

## Technology Stack

| Category | Technology |
|---|---|
| Language | C++17 |
| Graphics API | OpenGL 3.3 Core |
| Windowing/Input | GLFW |
| Mathematics | GLM |
| Model Loading | Assimp |
| UI | Dear ImGui |
| Image Loading | stb_image |
| OpenGL Loader | GLAD |
| Build System | CMake |
| Shader Language | GLSL |
| Audio | miniaudio |

---

## Project Structure

```text
IV_Project/
│
├── CMakeLists.txt
│
├── assets/
│   ├── models/
│   ├── textures/
│   └── ...
│
├── shaders/
│   ├── *.vert
│   ├── *.frag
│   └── ...
│
└── src/
    ├── main.cpp
    │
    ├── init.cpp
    ├── init.h
    │
    ├── camera.cpp
    ├── camera.h
    ├── third_person_camera.cpp
    ├── third_person_camera.h
    │
    ├── model.cpp
    ├── model.h
    ├── mesh.cpp
    ├── mesh.h
    │
    ├── shader.cpp
    ├── shader.h
    │
    ├── terrain_sampler.cpp
    ├── terrain_sampler.h
    │
    ├── player_controller.cpp
    ├── player_controller.h
    │
    ├── glad.c
    ├── glad/
    │
    ├── stb_image.*
    ├── miniaudio.*
    │
    └── imgui-master/
```

---

## Rendering Architecture

The application is organized around a conventional real-time rendering pipeline.

```text
Application
    │
    ├── GLFW Window & Input
    │
    ├── OpenGL Initialization
    │
    ├── Scene / Game State
    │
    ├── Camera
    │
    ├── Player Controller
    │
    └── Rendering
          │
          ├── Terrain
          ├── Environment
          ├── Characters
          ├── Instanced Objects
          └── UI
```

The main application loop is responsible for processing input, updating the game state, updating the camera and player, and rendering the scene each frame.

---

## Controls

| Action | Input |
|---|---|
| Move Forward | `W` |
| Move Left | `A` |
| Move Backward | `S` |
| Move Right | `D` |
| Sprint | `Left Shift` |
| Camera Look | Mouse |
| Normal Attack | Right Mouse Button |
| Normal Skill | `E` |
| Ultimate Skill | `Q` |
| Toggle Fullscreen | `F11` |
| Quit | `ESC` |

---

## Requirements

### Software

- CMake 3.15 or newer
- C++17-compatible compiler
- OpenGL 3.3 or newer
- Git
- Internet connection for downloading CMake dependencies

### Windows

Recommended:

- Windows 10/11
- Visual Studio 2019 or newer
- Visual Studio C++ Desktop Development workload

### Linux

A typical Linux environment requires:

- GCC or Clang
- CMake
- OpenGL development libraries
- X11/Wayland development libraries
- GLFW dependencies

For Arch Linux, the required packages may vary depending on the installed desktop environment and graphics stack.

---

## Building

### Linux

From the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Then run the executable generated inside the build directory.

For example:

```bash
./build/IV_Project
```

The exact executable location may depend on the CMake configuration.

---

### Windows

Using Visual Studio:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Then run:

```powershell
.\build\Release\IV_Project.exe
```

---

## Assets and Runtime Files

The application requires its asset and shader files at runtime.

The project uses:

```text
assets/
shaders/
```

If the CMake configuration copies these directories into the executable's output directory, the application can be launched directly from the generated build location.

If models or shaders cannot be found, verify that the executable has access to both directories.

---

## Important Source Files

### `src/main.cpp`

Application entry point and main game loop.

Responsible for coordinating:

- GLFW/OpenGL initialization
- ImGui
- Input handling
- Scene initialization
- Player updates
- Rendering
- Game/menu state

### `src/shader.*`

Shader abstraction used to:

- Load GLSL shader files
- Compile shaders
- Link shader programs
- Set shader uniforms

### `src/model.*`

Model loading and management using **Assimp**.

### `src/mesh.*`

Represents individual meshes and handles their OpenGL rendering resources.

### `src/player_controller.*`

Controls player movement, states, animation and gameplay-related behavior.

### `src/third_person_camera.*`

Implements the third-person camera system and camera movement/orbit behavior.

### `src/terrain_sampler.*`

Provides terrain-related sampling functionality used by the player and scene systems.

---

## Graphics Techniques

This project was developed with a particular focus on understanding real-time 3D rendering rather than relying on a complete game engine.

Some of the major graphics concepts explored include:

- Vertex Buffer Objects (VBO)
- Vertex Array Objects (VAO)
- Element Buffer Objects (EBO)
- GLSL shaders
- Model/View/Projection transformations
- Texture mapping
- Normal mapping and lighting techniques
- Depth testing
- Alpha blending
- Skeletal animation
- Third-person camera systems
- GPU instancing
- Framebuffer-based rendering techniques

---

## Development Goals

The project serves as both a playable application and an exploration of real-time graphics programming.

The main goals are:

1. Build a functioning 3D application using a low-level graphics API.
2. Understand how a 3D scene is transformed from model data into pixels on the screen.
3. Implement character rendering and animation.
4. Explore techniques for rendering large numbers of objects efficiently.
5. Experiment with different rendering architectures and optimization techniques.
6. Integrate rendering, gameplay, UI and supporting systems into a single application.

---

## Future Improvements

Possible future work includes:

- Improved rendering and lighting
- More advanced deferred/hybrid rendering
- Larger-scale instanced environments
- Improved animation blending
- More advanced enemy AI
- A* pathfinding
- Collision and physics improvements
- Network synchronization
- Persistent player/game data
- Rendering performance profiling and optimization

---

## Authors

**IV-BE CEIT Project Team**

| Name | Role | Primary Contribution |
|---|---|---|
| **Aung Myo Pai** | **Lead Developer & System Integrator** | Core rendering engine, rendering pipeline, instancing, skeletal animation, system integration and cross-module debugging |

---

## Academic Project

This project was developed as part of the **BE.CEIT final-year project** at:

**Technological University of Mandalay (TUM)**

The project focuses on practical implementation and experimentation with **real-time 3D graphics programming using C++ and OpenGL**.
