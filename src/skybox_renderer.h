#pragma once

#include <glm/glm.hpp>

#include <shader.h>

class SkyboxRenderer
{
public:
    SkyboxRenderer(const char* vertexShaderPath, const char* fragmentShaderPath);
    ~SkyboxRenderer();

    void SetColors(const glm::vec3& topColor, const glm::vec3& horizonColor, const glm::vec3& bottomColor);
    void Draw(const glm::mat4& view, const glm::mat4& projection) const;

private:
    unsigned int vao = 0;
    unsigned int vbo = 0;

    Shader shader;
    glm::vec3 topColor = glm::vec3(0.33f, 0.47f, 0.73f);
    glm::vec3 horizonColor = glm::vec3(0.62f, 0.70f, 0.84f);
    glm::vec3 bottomColor = glm::vec3(0.77f, 0.80f, 0.86f);
};
