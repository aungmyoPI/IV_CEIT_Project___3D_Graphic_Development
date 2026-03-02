#include <terrain_sampler.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

int TerrainSampler::GridIndex(int x, int z) const
{
    return (z * gridWidth) + x;
}

void TerrainSampler::BuildFromModel(const Model& terrainModel, const glm::vec3& translate, const glm::vec3& scale)
{
    ready = false;
    heightGrid.clear();
    validGrid.clear();
    gridWidth = 0;
    gridHeight = 0;

    minX = std::numeric_limits<float>::max();
    maxX = std::numeric_limits<float>::lowest();
    minZ = std::numeric_limits<float>::max();
    maxZ = std::numeric_limits<float>::lowest();
    minY = std::numeric_limits<float>::max();
    maxY = std::numeric_limits<float>::lowest();

    std::size_t vertexCount = 0;
    for (const Mesh& mesh : terrainModel.meshes)
        vertexCount += mesh.vertices.size();

    if (vertexCount == 0)
    {
        minY = 0.0f;
        maxY = 1.0f;
        std::cout << "[WARN] Terrain mesh vertices unavailable. Height collision disabled." << std::endl;
        return;
    }

    const float approxSide = std::sqrt(static_cast<float>(vertexCount));
    const int targetRes = std::clamp(static_cast<int>(approxSide * 0.60f), 128, 1024);
    gridWidth = targetRes;
    gridHeight = targetRes;

    for (const Mesh& mesh : terrainModel.meshes)
    {
        for (const Vertex& vertex : mesh.vertices)
        {
            const glm::vec3 worldVertex(
                translate.x + (vertex.Position.x * scale.x),
                translate.y + (vertex.Position.y * scale.y),
                translate.z + (vertex.Position.z * scale.z));

            if (worldVertex.x < minX)
                minX = worldVertex.x;
            if (worldVertex.x > maxX)
                maxX = worldVertex.x;
            if (worldVertex.z < minZ)
                minZ = worldVertex.z;
            if (worldVertex.z > maxZ)
                maxZ = worldVertex.z;
            if (worldVertex.y < minY)
                minY = worldVertex.y;
            if (worldVertex.y > maxY)
                maxY = worldVertex.y;
        }
    }

    const float spanX = maxX - minX;
    const float spanZ = maxZ - minZ;
    if (spanX <= 0.0001f || spanZ <= 0.0001f)
    {
        std::cout << "[WARN] Terrain bounds are degenerate. Height collision disabled." << std::endl;
        return;
    }

    const int cellCount = gridWidth * gridHeight;
    std::vector<float> accumHeight(cellCount, 0.0f);
    std::vector<std::uint32_t> accumCount(cellCount, 0U);

    for (const Mesh& mesh : terrainModel.meshes)
    {
        for (const Vertex& vertex : mesh.vertices)
        {
            const glm::vec3 worldVertex(
                translate.x + (vertex.Position.x * scale.x),
                translate.y + (vertex.Position.y * scale.y),
                translate.z + (vertex.Position.z * scale.z));

            const float u = (worldVertex.x - minX) / spanX;
            const float v = (worldVertex.z - minZ) / spanZ;
            const int x = std::clamp(static_cast<int>(std::lround(u * static_cast<float>(gridWidth - 1))), 0, gridWidth - 1);
            const int z = std::clamp(static_cast<int>(std::lround(v * static_cast<float>(gridHeight - 1))), 0, gridHeight - 1);
            const int idx = GridIndex(x, z);
            accumHeight[idx] += worldVertex.y;
            ++accumCount[idx];
        }
    }

    heightGrid.assign(cellCount, 0.0f);
    validGrid.assign(cellCount, 0);

    int validCells = 0;
    for (int i = 0; i < cellCount; ++i)
    {
        if (accumCount[i] == 0U)
            continue;
        heightGrid[i] = accumHeight[i] / static_cast<float>(accumCount[i]);
        validGrid[i] = 1;
        ++validCells;
    }

    if (validCells == 0)
    {
        std::cout << "[WARN] Terrain height grid build failed. Height collision disabled." << std::endl;
        return;
    }

    std::vector<float> nextHeight = heightGrid;
    std::vector<std::uint8_t> nextValid = validGrid;
    constexpr int kMaxFillPasses = 16;
    for (int pass = 0; pass < kMaxFillPasses; ++pass)
    {
        bool changed = false;
        for (int z = 0; z < gridHeight; ++z)
        {
            for (int x = 0; x < gridWidth; ++x)
            {
                const int idx = GridIndex(x, z);
                if (validGrid[idx] != 0)
                    continue;

                float sum = 0.0f;
                int count = 0;
                for (int dz = -1; dz <= 1; ++dz)
                {
                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        if (dx == 0 && dz == 0)
                            continue;
                        const int sx = x + dx;
                        const int sz = z + dz;
                        if (sx < 0 || sx >= gridWidth || sz < 0 || sz >= gridHeight)
                            continue;
                        const int sidx = GridIndex(sx, sz);
                        if (validGrid[sidx] == 0)
                            continue;
                        sum += heightGrid[sidx];
                        ++count;
                    }
                }
                if (count > 0)
                {
                    nextHeight[idx] = sum / static_cast<float>(count);
                    nextValid[idx] = 1;
                    changed = true;
                }
            }
        }

        heightGrid.swap(nextHeight);
        validGrid.swap(nextValid);
        if (!changed)
            break;
    }

    ready = true;
    std::cout << "[INFO] Terrain sampler grid built: " << gridWidth << "x" << gridHeight << std::endl;
}

bool TerrainSampler::IsReady() const
{
    return ready;
}

float TerrainSampler::SampleWorldHeight(float worldX, float worldZ) const
{
    if (!IsReady())
        return 0.0f;

    const float spanX = std::max(maxX - minX, 0.0001f);
    const float spanZ = std::max(maxZ - minZ, 0.0001f);
    const float u = std::clamp((worldX - minX) / spanX, 0.0f, 1.0f);
    const float v = std::clamp((worldZ - minZ) / spanZ, 0.0f, 1.0f);
    return SampleGridBilinear(u, v);
}

float TerrainSampler::SampleGridPoint(int x, int z) const
{
    x = std::clamp(x, 0, gridWidth - 1);
    z = std::clamp(z, 0, gridHeight - 1);
    const int center = GridIndex(x, z);
    if (validGrid[center] != 0)
        return heightGrid[center];

    constexpr int kSearchRadius = 6;
    float bestHeight = minY;
    float bestDist2 = std::numeric_limits<float>::max();
    for (int r = 1; r <= kSearchRadius; ++r)
    {
        bool foundInRadius = false;
        for (int dz = -r; dz <= r; ++dz)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                const int sx = x + dx;
                const int sz = z + dz;
                if (sx < 0 || sx >= gridWidth || sz < 0 || sz >= gridHeight)
                    continue;
                const int idx = GridIndex(sx, sz);
                if (validGrid[idx] == 0)
                    continue;
                const float dist2 = static_cast<float>(dx * dx + dz * dz);
                if (dist2 < bestDist2)
                {
                    bestDist2 = dist2;
                    bestHeight = heightGrid[idx];
                }
                foundInRadius = true;
            }
        }
        if (foundInRadius)
            return bestHeight;
    }
    return bestHeight;
}

float TerrainSampler::SampleGridBilinear(float u, float v) const
{
    const float fx = u * static_cast<float>(gridWidth - 1);
    const float fz = v * static_cast<float>(gridHeight - 1);
    const int x0 = static_cast<int>(std::floor(fx));
    const int z0 = static_cast<int>(std::floor(fz));
    const int x1 = std::min(x0 + 1, gridWidth - 1);
    const int z1 = std::min(z0 + 1, gridHeight - 1);

    const float tx = fx - static_cast<float>(x0);
    const float tz = fz - static_cast<float>(z0);

    const float h00 = SampleGridPoint(x0, z0);
    const float h10 = SampleGridPoint(x1, z0);
    const float h01 = SampleGridPoint(x0, z1);
    const float h11 = SampleGridPoint(x1, z1);

    const float hx0 = h00 + (h10 - h00) * tx;
    const float hx1 = h01 + (h11 - h01) * tx;
    return hx0 + (hx1 - hx0) * tz;
}

float TerrainSampler::GetMinY() const
{
    return minY;
}

float TerrainSampler::GetMaxY() const
{
    return maxY;
}
