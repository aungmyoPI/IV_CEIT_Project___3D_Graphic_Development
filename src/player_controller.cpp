#include <player_controller.h>

#include <filesystem.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <limits>

PlayerController::PlayerController()
    : position(0.0f, 0.0f, 0.0f),
      yawDeg(180.0f),
      groundOffsetY(0.0f),
      visualHeight(1.0f),
      radius(1.0f),
      isMoving(false),
      currentAction(PlayerAction::None),
      runFrameIndex(0),
      runAnimClock(0.0f),
      actionFrameIndex(0),
      actionAnimClock(0.0f),
      prevRightMouseDown(false),
      prevEDown(false),
      prevQDown(false),
      smoothedGroundY(0.0f),
      groundYInitialized(false)
{
}

void PlayerController::LoadModelIfExists(std::vector<Model>& out, const std::string& relativePath)
{
    const std::string absolutePath = FileSystem::getPath(relativePath);
    if (!std::filesystem::exists(absolutePath))
    {
        std::cout << "[WARN] Missing player model frame: " << absolutePath << std::endl;
        return;
    }
    out.emplace_back(absolutePath);
}

void PlayerController::LoadModelRangeIfExists(std::vector<Model>& out, const std::string& folder, const std::string& baseName, int startFrame, int endFrame)
{
    for (int i = startFrame; i <= endFrame; ++i)
    {
        const std::string relativePath = folder + "/" + baseName + std::to_string(i) + ".obj";
        LoadModelIfExists(out, relativePath);
    }
}

void PlayerController::LoadAllStates()
{
    normalAttack.clear();
    skillAttack.clear();
    ultimateAttack.clear();
    run.clear();
    death.clear();

    LoadModelRangeIfExists(normalAttack, "assets/Player/Player_fight", "CS_playerfight", 1, 12);

    LoadModelIfExists(skillAttack, "assets/Player/Player_fightbasic/CS_playerfightb.obj");
    LoadModelRangeIfExists(skillAttack, "assets/Player/Player_fightbasic", "CS_playerfightb", 1, 8);

    LoadModelRangeIfExists(ultimateAttack, "assets/Player/player_fightbasic2", "CS_playerfb", 1, 9);

    LoadModelIfExists(run, "assets/Player/Player_run/CS_prun.obj");
    LoadModelRangeIfExists(run, "assets/Player/Player_run", "CS_prun", 1, 11);

    LoadModelRangeIfExists(death, "assets/Player/Player_Death", "CS_pdeath", 1, 4);
}

PlayerController::ModelBounds PlayerController::CalculateModelBounds(const Model& model)
{
    ModelBounds bounds;
    if (model.meshes.empty())
        return bounds;

    glm::vec3 minPos(std::numeric_limits<float>::max());
    glm::vec3 maxPos(std::numeric_limits<float>::lowest());
    bool hasVertex = false;
    for (const Mesh& mesh : model.meshes)
    {
        for (const Vertex& vertex : mesh.vertices)
        {
            hasVertex = true;
            minPos = glm::min(minPos, vertex.Position);
            maxPos = glm::max(maxPos, vertex.Position);
        }
    }

    if (!hasVertex)
        return bounds;

    bounds.valid = true;
    bounds.min = minPos;
    bounds.max = maxPos;
    return bounds;
}

Model* PlayerController::GetReferenceFrame()
{
    if (!run.empty())
        return &run.front();
    if (!normalAttack.empty())
        return &normalAttack.front();
    if (!skillAttack.empty())
        return &skillAttack.front();
    if (!ultimateAttack.empty())
        return &ultimateAttack.front();
    if (!death.empty())
        return &death.front();
    return nullptr;
}

void PlayerController::InitializeGrounding(const TerrainSampler& terrain)
{
    if (Model* ref = GetReferenceFrame())
    {
        const ModelBounds bounds = CalculateModelBounds(*ref);
        if (bounds.valid)
        {
            const float localHeight = bounds.max.y - bounds.min.y;
            const float localWidth = bounds.max.x - bounds.min.x;
            const float localDepth = bounds.max.z - bounds.min.z;
            visualHeight = std::max(localHeight * kModelScale, 1.0f);
            radius = std::max(0.5f * std::max(localWidth, localDepth) * kModelScale, 1.0f);
            groundOffsetY = -bounds.min.y * kModelScale;
        }
    }

    if (terrain.IsReady())
    {
        position.y = terrain.SampleWorldHeight(position.x, position.z) + groundOffsetY + kFeetOffset;
        smoothedGroundY = position.y;
        groundYInitialized = true;
    }
    else
    {
        position.y = groundOffsetY + kFeetOffset;
        smoothedGroundY = position.y;
        groundYInitialized = true;
    }
}

std::vector<Model>* PlayerController::GetActionFrames(PlayerAction action)
{
    if (action == PlayerAction::NormalAttack)
        return &ultimateAttack;
    if (action == PlayerAction::SkillAttack)
        return &skillAttack;
    if (action == PlayerAction::UltimateAttack)
        return &normalAttack;
    return nullptr;
}

const std::vector<Model>* PlayerController::GetActionFrames(PlayerAction action) const
{
    if (action == PlayerAction::NormalAttack)
        return &normalAttack;
    if (action == PlayerAction::SkillAttack)
        return &skillAttack;
    if (action == PlayerAction::UltimateAttack)
        return &ultimateAttack;
    return nullptr;
}

void PlayerController::HandleActionInput(GLFWwindow* window)
{
    const bool rightMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    const bool eDown = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
    const bool qDown = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;

    if (qDown && !prevQDown && !ultimateAttack.empty())
    {
        currentAction = PlayerAction::UltimateAttack;
        actionFrameIndex = 0;
        actionAnimClock = 0.0f;
    }
    else if (eDown && !prevEDown && !skillAttack.empty())
    {
        currentAction = PlayerAction::SkillAttack;
        actionFrameIndex = 0;
        actionAnimClock = 0.0f;
    }
    else if (rightMouseDown && !prevRightMouseDown && !normalAttack.empty())
    {
        currentAction = PlayerAction::NormalAttack;
        actionFrameIndex = 0;
        actionAnimClock = 0.0f;
    }

    prevRightMouseDown = rightMouseDown;
    prevEDown = eDown;
    prevQDown = qDown;
}

void PlayerController::UpdateMovement(GLFWwindow* window, const Camera& camera, float deltaTime)
{
    glm::vec3 cameraForwardFlat(camera.Front.x, 0.0f, camera.Front.z);
    if (glm::dot(cameraForwardFlat, cameraForwardFlat) < kMinVectorLenSq)
    {
        const float yawRad = glm::radians(camera.Yaw);
        cameraForwardFlat = glm::normalize(glm::vec3(std::cos(yawRad), 0.0f, std::sin(yawRad)));
    }
    else
    {
        cameraForwardFlat = glm::normalize(cameraForwardFlat);
    }
    const glm::vec3 cameraRightFlat = glm::normalize(glm::cross(cameraForwardFlat, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 moveInput(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveInput += cameraForwardFlat;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveInput -= cameraForwardFlat;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveInput -= cameraRightFlat;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveInput += cameraRightFlat;

    isMoving = glm::dot(moveInput, moveInput) > kMinVectorLenSq;
    if (isMoving)
    {
        const glm::vec3 moveDir = glm::normalize(moveInput);
        const bool sprintHeld = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        const float speed = kMoveSpeed * (sprintHeld ? kSprintMultiplier : 1.0f);
        position += moveDir * (speed * deltaTime);
        yawDeg = glm::degrees(std::atan2(moveDir.x, moveDir.z)) + kModelYawOffsetDeg;

        runAnimClock += deltaTime;
        if (!run.empty())
        {
            const float frameTime = 1.0f / kRunAnimationFps;
            while (runAnimClock >= frameTime)
            {
                runAnimClock -= frameTime;
                runFrameIndex = (runFrameIndex + 1) % run.size();
            }
        }
    }
    else
    {
        runAnimClock = 0.0f;
        runFrameIndex = 0;
    }
}

void PlayerController::UpdateActionAnimation(float deltaTime)
{
    if (currentAction == PlayerAction::None)
        return;

    const std::vector<Model>* frames = GetActionFrames(currentAction);
    if (frames == nullptr || frames->empty())
    {
        currentAction = PlayerAction::None;
        actionFrameIndex = 0;
        actionAnimClock = 0.0f;
        return;
    }

    actionAnimClock += deltaTime;
    const float frameTime = 1.0f / kAttackAnimationFps;
    while (actionAnimClock >= frameTime)
    {
        actionAnimClock -= frameTime;
        ++actionFrameIndex;
        if (actionFrameIndex >= frames->size())
        {
            currentAction = PlayerAction::None;
            actionFrameIndex = 0;
            actionAnimClock = 0.0f;
            break;
        }
    }
}

void PlayerController::ApplyGrounding(const TerrainSampler& terrain, float deltaTime)
{
    if (terrain.IsReady())
    {
        const float probeR = 0.85f;
        const float centerY = terrain.SampleWorldHeight(position.x, position.z);
        const float pxY = terrain.SampleWorldHeight(position.x + probeR, position.z);
        const float nxY = terrain.SampleWorldHeight(position.x - probeR, position.z);
        const float pzY = terrain.SampleWorldHeight(position.x, position.z + probeR);
        const float nzY = terrain.SampleWorldHeight(position.x, position.z - probeR);
        const float targetTerrainY = centerY * 0.40f + (pxY + nxY + pzY + nzY) * 0.15f;
        const float targetY = targetTerrainY + groundOffsetY + kFeetOffset;

        if (!groundYInitialized)
        {
            smoothedGroundY = targetY;
            groundYInitialized = true;
        }

        if (targetY >= smoothedGroundY)
        {
            // Never lag behind terrain while moving uphill; prevents clipping into ground.
            smoothedGroundY = targetY;
        }
        else
        {
            // Descending can stay smoothed to avoid jitter.
            const float followRate = isMoving ? 14.0f : 18.0f;
            const float blend = std::clamp(deltaTime * followRate, 0.0f, 1.0f);
            smoothedGroundY += (targetY - smoothedGroundY) * blend;
            if (std::abs(targetY - smoothedGroundY) < 0.01f)
                smoothedGroundY = targetY;
        }

        position.y = smoothedGroundY;
    }
}

void PlayerController::Update(GLFWwindow* window, const Camera& camera, float deltaTime, const TerrainSampler& terrain)
{
    HandleActionInput(window);
    UpdateMovement(window, camera, deltaTime);
    UpdateActionAnimation(deltaTime);
    ApplyGrounding(terrain, deltaTime);
}

Model* PlayerController::GetCurrentFrame()
{
    if (std::vector<Model>* actionFrames = GetActionFrames(currentAction))
    {
        if (!actionFrames->empty())
            return &(*actionFrames)[actionFrameIndex % actionFrames->size()];
    }

    if (isMoving && !run.empty())
        return &run[runFrameIndex % run.size()];
    if (!run.empty())
        return &run.front();
    if (!normalAttack.empty())
        return &normalAttack.front();
    if (!skillAttack.empty())
        return &skillAttack.front();
    if (!ultimateAttack.empty())
        return &ultimateAttack.front();
    if (!death.empty())
        return &death.front();
    return nullptr;
}

const glm::vec3& PlayerController::GetPosition() const
{
    return position;
}

float PlayerController::GetYawDeg() const
{
    return yawDeg;
}

float PlayerController::GetModelScale() const
{
    return kModelScale;
}

float PlayerController::GetVisualHeight() const
{
    return visualHeight;
}

float PlayerController::GetRadius() const
{
    return radius;
}

bool PlayerController::IsMoving() const
{
    return isMoving;
}
