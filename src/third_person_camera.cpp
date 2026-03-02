#include <third_person_camera.h>

#include <algorithm>

ThirdPersonCameraRig::ThirdPersonCameraRig(float baseDistance, float baseTargetHeight, float groundClearance)
    : baseDistance(baseDistance),
      baseTargetHeight(baseTargetHeight),
      groundClearance(groundClearance),
      followDistance(baseDistance),
      followTargetHeight(baseTargetHeight)
{
}

void ThirdPersonCameraRig::ConfigureFromPlayerDimensions(float playerVisualHeight, float playerRadius)
{
    followTargetHeight = std::max(baseTargetHeight, playerVisualHeight * 0.68f);
    followDistance = std::max(baseDistance, playerRadius * 2.8f + 3.5f);
}

void ThirdPersonCameraRig::Update(Camera& camera, const glm::vec3& playerPos, const TerrainSampler& terrain) const
{
    const glm::vec3 cameraAnchor = playerPos + glm::vec3(0.0f, followTargetHeight, 0.0f);
    glm::vec3 desiredCameraPos = cameraAnchor - camera.Front * followDistance;

    if (terrain.IsReady())
    {
        const float terrainYAtCamera = terrain.SampleWorldHeight(desiredCameraPos.x, desiredCameraPos.z);
        const float minCameraY = terrainYAtCamera + groundClearance;
        if (desiredCameraPos.y < minCameraY)
            desiredCameraPos.y = minCameraY;
    }

    camera.Position = desiredCameraPos;
}
