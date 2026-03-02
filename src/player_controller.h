#pragma once

#include <camera.h>
#include <glfw/glfw3.h>
#include <model.h>
#include <terrain_sampler.h>

#include <string>
#include <vector>

class PlayerController
{
public:
    PlayerController();

    void LoadAllStates();
    void InitializeGrounding(const TerrainSampler& terrain);
    void Update(GLFWwindow* window, const Camera& camera, float deltaTime, const TerrainSampler& terrain);

    Model* GetCurrentFrame();

    const glm::vec3& GetPosition() const;
    float GetYawDeg() const;
    float GetModelScale() const;
    float GetVisualHeight() const;
    float GetRadius() const;
    bool IsMoving() const;

private:
    struct ModelBounds
    {
        bool valid = false;
        glm::vec3 min = glm::vec3(0.0f);
        glm::vec3 max = glm::vec3(0.0f);
    };

    enum class PlayerAction
    {
        None,
        NormalAttack,
        SkillAttack,
        UltimateAttack
    };

    static constexpr float kMoveSpeed = 70.0f;
    static constexpr float kSprintMultiplier = 1.65f;
    static constexpr float kFeetOffset = 0.1f;
    static constexpr float kModelScale = 8.0f;
    static constexpr float kModelYawOffsetDeg = 0.0f;
    static constexpr float kRunAnimationFps = 12.0f;
    static constexpr float kAttackAnimationFps = 16.0f;
    static constexpr float kMinVectorLenSq = 0.000001f;

    glm::vec3 position;
    float yawDeg;

    float groundOffsetY;
    float visualHeight;
    float radius;

    bool isMoving;
    PlayerAction currentAction;
    size_t runFrameIndex;
    float runAnimClock;
    size_t actionFrameIndex;
    float actionAnimClock;
    bool prevRightMouseDown;
    bool prevEDown;
    bool prevQDown;
    float smoothedGroundY;
    bool groundYInitialized;

    std::vector<Model> normalAttack;
    std::vector<Model> skillAttack;
    std::vector<Model> ultimateAttack;
    std::vector<Model> run;
    std::vector<Model> death;

    void HandleActionInput(GLFWwindow* window);
    void UpdateMovement(GLFWwindow* window, const Camera& camera, float deltaTime);
    void UpdateActionAnimation(float deltaTime);
    void ApplyGrounding(const TerrainSampler& terrain, float deltaTime);

    static ModelBounds CalculateModelBounds(const Model& model);
    void LoadModelIfExists(std::vector<Model>& out, const std::string& relativePath);
    void LoadModelRangeIfExists(std::vector<Model>& out, const std::string& folder, const std::string& baseName, int startFrame, int endFrame);
    std::vector<Model>* GetActionFrames(PlayerAction action);
    const std::vector<Model>* GetActionFrames(PlayerAction action) const;
    Model* GetReferenceFrame();
};
