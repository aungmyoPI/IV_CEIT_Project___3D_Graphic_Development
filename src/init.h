#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <Config.h>
#include <callback.h>

GLFWwindow* init();
void cerr_t(const char *what);
void cout_t(const char *what);