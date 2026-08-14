# IV_Project — 3D Graphic Development (IV-BE CEIT Project)

A desktop 3D application / small game demo built with C++17, OpenGL 3.3, GLFW, ImGui and Assimp for model loading. The app renders a terrain and playable character model, uses an immediate-mode UI for the main menu, and supports basic player movement and camera rigs.

## Highlights
- Real-time terrain rendering with multiple blended terrain textures.
- Player controller with multiple animation states and third-person camera rig.
- Uses Assimp for .glb model loading; stb_image + stb for textures; miniaudio present for audio (header included).
- ImGui-based start menu and guide overlay.

## Stack
- Language(s): C++ (primary), C
- Build: CMake (>= 3.15)
- Runtime / APIs: OpenGL 3.3 (GLSL "#version 330"), GLFW windowing/input
- Notable libraries:
  - GLFW (fetched via CMake FetchContent)
  - GLM (math, fetched via FetchContent)
  - Assimp (model loading, fetched via FetchContent)
  - Dear ImGui (bundled under `src/imgui-master`)
  - stb_image, miniaudio included in `src/`

## Repository layout
```
CMakeLists.txt                 # top-level build script
assets/                        # textures, models, logo, etc. (copied to build dir)
shaders/                       # GLSL vertex/fragment shader files
src/                           # application source
  main.cpp                     # application entrypoint, UI, main loop, controls
  init.cpp / init.h            # OpenGL / GLFW initialization
  camera.* / third_person_camera.*  # camera rigs and utilities
  model.* / mesh.*             # model/mesh handling (Assimp wrappers)
  shader.*                     # shader loader/wrapper
  terrain_sampler.*            # builds terrain sampling structure from model
  player_controller.*          # player state, animation & update logic
  imgui-master/                # Dear ImGui sources (backends + examples)
  stb_image.* , miniaudio.*    # single-file libs included
  glad.c / glad/               # OpenGL loader
```

How it fits together:
- The executable is named `IV_Project` (CMake PROJECT_NAME). CMake gathers all `src/*.cpp` (excluding `imgui-master` internals), builds `imgui_lib` from the included ImGui files and links GLFW, GLM and Assimp.
- At runtime main.cpp initializes GLFW + OpenGL + ImGui, loads the terrain model (via Assimp), builds a TerrainSampler, loads player models and textures, and then enters the main loop which either shows the UI menu (ImGui) or updates/ draws the game world.

## Controls (as implemented in `src/main.cpp`)
- Move: W A S D
- Sprint: Left Shift (hold)
- Camera look: Mouse move
- Normal attack: Right mouse button
- Normal skill: E
- Ultimate skill: Q
- Toggle fullscreen: F11
- Quit game: ESC

## Requirements
- CMake >= 3.15
- C++17-capable compiler:
  - GCC/Clang (Linux/macOS)
  - MSVC (Visual Studio 2019/2022) on Windows
- OpenGL 3.3+ driver on the system / GPU
- On Linux: X11 / Wayland development packages and OpenGL dev packages (see platform-specific note below)
- Network access for CMake to FetchContent (it downloads GLFW, GLM, Assimp automatically)

## Build and run

From repository root (recommended out-of-source build):

Linux / macOS (Unix-like)
```bash
# Create a build folder and configure
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
# Build (parallel)
cmake --build . --config Release -- -j$(nproc)
# The executable will be in the build output folder; run it
./IV_Project    # or on some systems: ./IV_Project.exe
```

Windows (Visual Studio)
```powershell
# From repo root (PowerShell)
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
# Run the built exe:
.\build\Release\IV_Project.exe
```

Notes:
- CMakeLists uses FetchContent to auto-download GLFW, GLM and Assimp. If you prefer system packages or vcpkg, modify the CMake file accordingly.
- CMake contains a post-build step that copies `assets/` and `shaders/` into the executable directory, so run the executable from the build output directory or ensure `assets/` and `shaders/` are accessible at runtime.

Platform-specific package hints (common on Linux):
- Debian/Ubuntu example packages you may need to install before building:
  - build-essential, cmake, libgl1-mesa-dev, libx11-dev, libxrandr-dev, libxinerama-dev, libxcursor-dev, libxi-dev, libasound2-dev, libglm-dev (optional if letting CMake FetchContent)
- On macOS: install Xcode command line tools and ensure you have an OpenGL-capable driver (or use Homebrew SDL/GLFW equivalents).

## Common issues & troubleshooting
- Shader compile errors: The app requests GLSL `#version 330`. If your system uses a different core profile, edit `ImGui_ImplOpenGL3_Init("#version 330");` or the shader files in `shaders/`.
- Missing OpenGL or windowing libs: on Linux, install the X11/GL development headers listed above.
- Linker errors with MSVC: CMake sets `CMAKE_MSVC_RUNTIME_LIBRARY` to a static runtime in the project. If you get CRT mismatch errors, either adjust the CMakeLists or build using the same runtime settings for all linked libs.
- Models/textures not found at runtime: ensure `assets/` and `shaders/` are present next to the executable (CMake copy step should handle this on successful build).

## Files of interest (quick pointers)
- `CMakeLists.txt` — top-level build configuration: fetches dependencies (GLFW, GLM, Assimp), compiles ImGui backend and copies assets/shaders post-build.
- `src/main.cpp` — app entry, ImGui menu, main loop and control mapping.
- `src/model.*`, `src/mesh.*` — model loading & drawing using Assimp.
- `src/terrain_sampler.*` — builds sampling structure used for terrain height lookups and texturing.
- `src/player_controller.*` — player states / animation frames and update logic.
- `shaders/` — GLSL vertex/fragment shaders used by the terrain, player and skybox.

## Running the distributed binary
Your repository also includes a Drive link in the old README; that appears to be a packaged binary. If you use the packaged binary, ensure you run it from the folder that contains `assets/` and `shaders/` or that they are present relative to the executable.

## Contributing & notes
- To fix build or runtime issues, open an issue with the console output and platform details (OS, GPU, driver version).
- If you change engine-level interfaces (Shader, Model, TerrainSampler), update examples in `src/` accordingly.
- Consider adding a LICENSE file (currently no explicit project license found). If you want permissive licensing, MIT is commonly used for student projects.

## Next steps / Suggested improvements
- Add CI that builds on Linux/Windows to verify dependency fetch + build.
- Expose a command-line flag to choose window size / fullscreen / debug shaders.
- Split third-party libs (ImGui/Glad/stb/miniaudio) into `third_party/` and add a small script to update them.

## Try asking
- "How do I change the GLSL version if my GPU doesn't support 330?" — look at `ImGui_ImplOpenGL3_Init("#version 330")` and the shader files in `shaders/`.
- "Where is the terrain height range computed?" — see `src/terrain_sampler.cpp` and the calls to `TerrainSampler::BuildFromModel()` in `src/main.cpp`.
- "How are player animation frames stored and switched?" — check `src/player_controller.h/.cpp` and how `PlayerController::GetCurrentFrame()` is used before drawing.
