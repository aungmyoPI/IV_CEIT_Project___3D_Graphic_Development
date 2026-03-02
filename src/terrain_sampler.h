#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

#include <model.h>

class TerrainSampler
{
public:
    void BuildFromModel(const Model& terrainModel, const glm::vec3& translate, const glm::vec3& scale);
    bool IsReady() const;
    float SampleWorldHeight(float worldX, float worldZ) const;
    float GetMinY() const;
    float GetMaxY() const;

private:
    int gridWidth = 0;
    int gridHeight = 0;
    std::vector<float> heightGrid;
    std::vector<std::uint8_t> validGrid;
    float minX = 0.0f;
    float maxX = 0.0f;
    float minZ = 0.0f;
    float maxZ = 0.0f;
    float minY = 0.0f;
    float maxY = 0.0f;
    bool ready = false;

    int GridIndex(int x, int z) const;
    float SampleGridPoint(int x, int z) const;
    float SampleGridBilinear(float u, float v) const;
};
