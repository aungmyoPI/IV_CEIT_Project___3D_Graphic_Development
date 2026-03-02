cmake_minimum_required(VERSION 3.15)
project(IV_Project)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# --- 1. Dependencies (Auto-Download) ---

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.3.8)

# GLM
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)

# Assimp - Using GIT_SHALLOW to fix the "Early EOF" download error
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    assimp 
    GIT_REPOSITORY https://github.com/assimp/assimp.git 
    GIT_TAG v5.3.1
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(glfw glm assimp)

# --- 2. ImGui Configuration (Using your src/imgui-master folder) ---

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/imgui-master")

# We manually pick ONLY the files needed for GLFW + OpenGL3
add_library(imgui_lib STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Tell the compiler where the ImGui headers are
target_include_directories(imgui_lib PUBLIC 
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
)

# ImGui backends need to link to GLFW
target_link_libraries(imgui_lib PRIVATE glfw)

# --- 3. Source Management (The Filter) ---

# Get all files in src, but EXCLUDE the imgui-master directory 
# so we don't try to compile the Allegro/Android backends.
file(GLOB_RECURSE ALL_SRCS "src/*.cpp" "src/*.c")

set(SOURCES "")
foreach(FILE ${ALL_SRCS})
    if(NOT FILE MATCHES "imgui-master")
        list(APPEND SOURCES ${FILE})
    endif()
endforeach()

add_executable(${PROJECT_NAME} ${SOURCES})

# --- 4. Configuration & Linking ---

target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link your main app with all libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    glm::glm 
    assimp::assimp
    imgui_lib
)

# --- 5. Asset Sync ---
foreach(DIR assets shaders)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DIR}"
    )
endforeach()