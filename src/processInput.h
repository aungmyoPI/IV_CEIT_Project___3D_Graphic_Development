#pragma once

#include <Config.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <camera.h>

inline Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
inline float lastX = SCR_WIDTH / 2.0f;
inline float lastY = SCR_HEIGHT / 2.0f;
inline bool firstMouse = true;

void processInput(GLFWwindow *window);