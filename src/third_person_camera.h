#pragma once

#include <camera.h>
#include <terrain_sampler.h>

class ThirdPersonCameraRig
{
public:
    ThirdPersonCameraRig(float baseDistance, float baseTargetHeight, float groundClearance);

    void ConfigureFromPlayerDimensions(float playerVisualHeight, float playerRadius);
    void Update(Camera& camera, const glm::vec3& playerPos, const TerrainSampler& terrain) const;

private:
    float baseDistance;
    float baseTargetHeight;
    float groundClearance;

    float followDistance;
    float followTargetHeight;
};
