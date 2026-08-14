# IV_Project: Cyber Samurai — 3D Graphics Development

<p align="center">
  <b>A real-time 3D action-adventure game blending traditional samurai culture with futuristic cyber-technology.</b>
</p>

<p align="center">
  <img src="assets/screenshot_placeholder.png" alt="Cyber Samurai Gameplay Preview" width="100%"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-blue.svg" alt="C++17">
  <img src="https://img.shields.io/badge/Graphics-OpenGL%203.3-green.svg" alt="OpenGL 3.3">
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg" alt="Platform">
  <img src="https://img.shields.io/badge/Build-CMake-064F8C.svg" alt="CMake">
  <img src="https://img.shields.io/badge/License-Academic-orange.svg" alt="Academic License">
</p>

---

## 📖 Overview

**Cyber Samurai** is a small playable 3D action-adventure application developed from the ground up using **C++ and OpenGL**.

Developed as a final-year project for the **Department of Information Technology Engineering, Technological University (Mandalay)**, the project explores the practical implementation of a real-time 3D graphics pipeline together with gameplay systems.

The project brings together:

- Real-time OpenGL rendering
- 3D terrain and environment rendering
- Skeletal character animation
- Third-person camera and player movement
- GPU instanced rendering
- Animation-driven combat
- Enemy interaction and gameplay state management
- Dear ImGui-based user interfaces
- Asset loading and texture management

The primary goal is to build a complete, playable application while gaining hands-on experience with the systems that form the foundation of modern real-time graphics.

---

## ✨ Features

### 🎨 Real-Time 3D Rendering

- OpenGL 3.3 Core rendering pipeline
- 3D terrain rendering with multi-texture blending
- Phong-based lighting
- Skybox environment rendering
- Model loading through **Assimp**
- Texture loading through **stb_image**
- Depth testing and standard real-time rendering optimizations

### 🧍 Player & Character System

- Playable 3D character with multiple animation states
- Responsive third-person camera
- Camera-relative WASD movement
- Dynamic character rotation based on movement direction
- Sprinting
- Skeletal animation playback

### ⚔️ Combat System

- Normal attack
- Skill and ultimate skill actions
- Animation-driven combat state management
- Player-to-enemy interaction
- Health and damage handling
- Basic win/lose and game-state conditions

### 🌲 GPU Instanced Rendering

The project uses **GPU instancing** to efficiently render large numbers of repeated environmental objects.

Examples include:

- Instanced vegetation and foliage
- Repeated environmental objects
- Large quantities of visually similar scene elements

Instancing reduces the CPU overhead associated with issuing separate draw calls for every repeated object.

### 🖥️ User Interface

- Dear ImGui-based main menu
- Game start interface
- In-game guide and HUD overlays
- Interactive configuration and debugging panels

---

## 🧱 Technology Stack

| Category | Technology |
|---|---|
| Language | **C++17** |
| Graphics API | **OpenGL 3.3 Core** |
| Windowing & Input | **GLFW** |
| Mathematics | **GLM** |
| Model Loading | **Assimp** |
| UI Framework | **Dear ImGui** |
| Image Decoding | **stb_image** |
| OpenGL Loader | **GLAD** |
| Build System | **CMake** |
| Shader Language | **GLSL** |
| Audio | **miniaudio** |

---

## 🏗️ Project Structure

```text
IV_Project/
│
├── CMakeLists.txt
│
├── assets/
│   ├── models/
│   ├── textures/
│   ├── audio/
│   └── ...
│
├── shaders/
│   ├── *.vert
│   ├── *.frag
│   └── ...
│
└── src/
    ├── main.cpp
    ├── init.cpp / .h
    ├── camera.cpp / .h
    ├── third_person_camera.cpp / .h
    ├── model.cpp / .h
    ├── mesh.cpp / .h
    ├── shader.cpp / .h
    ├── terrain_sampler.cpp / .h
    ├── player_controller.cpp / .h
    └── ...
```

---

## 🎬 Rendering Architecture

The application follows a conventional real-time rendering loop that coordinates input processing, game-state updates, camera and player transformations, and scene rendering.

```text
Application Loop
│
├── GLFW Window & Input Polling
│
├── Game State & Delta Time
│
├── Camera & Player Controller Updates
│
└── Rendering
    │
    ├── Terrain
    ├── Environment & Skybox
    ├── Characters
    │   ├── Player
    │   └── Enemies
    ├── Instanced Vegetation
    └── Dear ImGui Interface
```

---

## 🎮 Controls

| Action | Input |
|---|---|
| Move Forward | `W` |
| Move Left | `A` |
| Move Backward | `S` |
| Move Right | `D` |
| Sprint | `Left Shift` |
| Camera Look | `Mouse` |
| Normal Attack | `Right Mouse Button` |
| Normal Skill | `E` |
| Ultimate Skill | `Q` |
| Toggle Fullscreen | `F11` |
| Quit Application | `ESC` |

---

## 🔧 Requirements

### Software

- **CMake 3.15+**
- A **C++17-compatible compiler**
  - GCC
  - Clang
  - MSVC / Visual Studio 2022
- Git
- An OpenGL 3.3-compatible graphics card and driver

### Platform

The project is intended to run on:

- Windows
- Linux

---

## 🚀 Building

### Linux

Configure the project:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Build:

```bash
cmake --build build -j$(nproc)
```

Run:

```bash
./build/IV_Project
```

### Windows — Visual Studio 2022

Configure:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

Build:

```powershell
cmake --build build --config Release
```

Run:

```powershell
.\build\Release\IV_Project.exe
```

> **Note:** Make sure the `assets/` and `shaders/` directories are accessible from the application's runtime working directory. If the executable cannot find models, textures, or shaders, check the configured resource paths.

---

## 🔮 Future Improvements

The project can be extended in several directions:

- [ ] Full combo-based combat system
- [ ] Blocking and parry mechanics
- [ ] More advanced enemy AI and state-based behaviors
- [ ] A* pathfinding and environment-aware navigation
- [ ] Multiplayer player-vs-player synchronization
- [ ] More advanced dynamic lighting
- [ ] Particle effects and environmental VFX
- [ ] Additional levels and gameplay content
- [ ] Further rendering and performance optimization

---

## 👥 Project Team & Credits

Developed as a final-year project by **Group II** at the **Department of Information Technology Engineering, Technological University (Mandalay)**.

### Supervisor

**Daw Moh Moh Khaing**  
Associate Professor

### Team

| Name | Role |
|---|---|
| **Mg Aung Myo Paing** | Project Leader & Lead Developer |
| **Ma Thoon Thiri Swe** | Team Member |
| **Mg Nay Phone Myint** | Team Member |
| **Mg Kaung Khant Ko Ko** | Team Member |
| **Ma Khin Yadanar Win** | Team Member |
| **Mg Hlawn Moe Aung** | Team Member |

---

## 📚 Academic Context

**Project:** Cyber Samurai  
**Project Type:** Final-Year Project  
**Department:** Information Technology Engineering  
**University:** Technological University (Mandalay)  
**Group:** II  
**Academic Year:** Final Year

---

## 📄 License

This project was developed for **academic and educational purposes** as part of a final-year university project.

Third-party assets, libraries, models, textures, and other resources remain subject to their respective licenses and original creators' rights.
