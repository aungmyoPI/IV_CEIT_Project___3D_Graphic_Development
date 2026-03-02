#pragma once

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

struct Vertex{
    //position
    glm::vec3 Position;
    //normal
    glm::vec3 Normal;
    //texCoords
    glm::vec2 TexCoords;
    //tangent
    glm::vec3 Tangent;
    //bitangent
    glm::vec3 Bitangent;
    
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh{
    public:
    std::vector<Vertex>         vertices;
    std::vector<unsigned int>   indices;
    std::vector<Texture>        textures;
    glm::vec3                   diffuseColor;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, const glm::vec3& diffuseColor = glm::vec3(1.0f));

    void Draw(Shader& shader);

    private:
    unsigned int VBO, EBO;

    void setupMesh();
};
