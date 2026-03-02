/*__STANDARD_LIBS__*/
#include <stb_image.h>
#include <algorithm>
#include <array>
#include <iostream>
#include <utility>

/*__USER_DEFINED_LIBS__*/
#include <camera.h>
#include <filesystem.h>
#include <glm/gtc/matrix_transform.hpp>
#include <init.h>
#include <model.h>
#include <player_controller.h>
#include <processInput.h>
#include <shader.h>
#include <skybox_renderer.h>
#include <terrain_sampler.h>
#include <third_person_camera.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace
{
const glm::vec3 kTerrainTranslate(0.0f, -50.0f, 0.0f);
const glm::vec3 kTerrainScale(1500.0f, 1500.0f, 1500.0f);
const float kNearPlane = 0.1f;
const float kFarPlane = 10000.0f;
}

enum AppState
{
    Home,
    How_to_Play
};

bool gameStarted = false;

unsigned int loadTexture(char const *path);

int main()
{
    GLFWwindow* window = init();
    if (window == nullptr){
        return -1;
    }

    // setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    unsigned int logoTexture = loadTexture("assets/logo/logo.jpg");

    stbi_set_flip_vertically_on_load(true);  
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    Shader terrainShader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    Shader playerShader("shaders/playerEnemyVertex.vert", "shaders/playerEnemyFrag.frag");
    SkyboxRenderer skybox("shaders/skybox.vert", "shaders/skybox.frag");
    const glm::vec3 fogColor(0.56f, 0.63f, 0.75f);
    skybox.SetColors(glm::vec3(0.25f, 0.41f, 0.72f), fogColor, glm::vec3(0.70f, 0.74f, 0.82f));

    Model terrainModel(FileSystem::getPath("assets/heightMap1.glb"));

    TerrainSampler terrainSampler;
    terrainSampler.BuildFromModel(terrainModel, kTerrainTranslate, kTerrainScale);
    std::cout << "[INFO] Terrain height range worldY: min=" << terrainSampler.GetMinY()
              << " max=" << terrainSampler.GetMaxY() << std::endl;

    PlayerController player;
    player.LoadAllStates();
    player.InitializeGrounding(terrainSampler);

    ThirdPersonCameraRig cameraRig(35.0f, 10.0f, 1.75f);
    cameraRig.ConfigureFromPlayerDimensions(player.GetVisualHeight(), player.GetRadius());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const unsigned int sandTex = terrainModel.TextureFromFile("sand.jpg", FileSystem::getPath("assets"));
    const unsigned int grassTex = terrainModel.TextureFromFile("grass.jpg", FileSystem::getPath("assets"));
    const unsigned int groundTex = terrainModel.TextureFromFile("rock.jpg", FileSystem::getPath("assets"));
    const unsigned int snowTex = terrainModel.TextureFromFile("snow.jpg", FileSystem::getPath("assets"));

    terrainShader.use();
    terrainShader.setInt("texture_sand", 10);
    terrainShader.setInt("texture_grass", 11);
    terrainShader.setInt("texture_ground", 12);
    terrainShader.setInt("texture_snow", 13);
    terrainShader.setFloat("terrainMinY", terrainSampler.GetMinY());
    terrainShader.setFloat("terrainMaxY", terrainSampler.GetMaxY());
    terrainShader.setFloat("terrainTexTiling", 12.0f);
    terrainShader.setFloat("terrainWorldTexScale", 0.018f);
    // Terrain texture distribution tuning (0..1 in normalized terrain height).
    // Order + rough coverage from bottom to top: sand(20%), rock(20%), grass(35%), snow(15%).
    terrainShader.setFloat("sandMax", 0.20f);
    terrainShader.setFloat("rockStart", 0.20f);
    terrainShader.setFloat("rockEnd", 0.40f);
    terrainShader.setFloat("grassStart", 0.40f);
    terrainShader.setFloat("grassEnd", 0.65f);
    terrainShader.setFloat("snowStart", 0.65f);
    terrainShader.setFloat("snowEnd", 0.85f);
    terrainShader.setFloat("rockSlopeBoost", 0.12f);

    camera.Yaw = -90.0f;
    camera.Pitch = -20.0f;
    camera.ProcessMouseMovement(0.0f, 0.0f);
    cameraRig.Update(camera, player.GetPosition(), terrainSampler);

    AppState currentPage = AppState::Home;

    while (!glfwWindowShouldClose(window))
    {
        const float currentFrame = static_cast<float>(glfwGetTime());
        Config::deltaTime = currentFrame - Config::lastFrame;
        Config::lastFrame = currentFrame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!gameStarted)
        {
            static bool f11Pressed = false;
            if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
            {
                if (!f11Pressed)
                {
                    static bool isFullscreen = false;
                    static int savedWidth = SCR_WIDTH;
                    static int savedHeight = SCR_HEIGHT;
                    static int savedXPos = 0;
                    static int savedYPos = 0;

                    if (!isFullscreen)
                    {
                        glfwGetWindowPos(window, &savedXPos, &savedYPos);
                        glfwGetWindowSize(window, &savedWidth, &savedHeight);

                        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
                        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
                        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                        isFullscreen = true;
                    }
                    else
                    {
                        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
                        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
                        int centerX = (mode->width - savedWidth) / 2;
                        int centerY = (mode->height - savedHeight) / 2;
                        glfwSetWindowMonitor(window, NULL, centerX, centerY, savedWidth, savedHeight, mode->refreshRate);
                        isFullscreen = false;
                    }

                    f11Pressed = true;
                }
            }
            else
            {
                f11Pressed = false;
            }

            // 1. Push Global Window Styles (Count: 2)
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ... (Your F11 Fullscreen logic remains here) ...

            // 2. Begin the Window (Shared by both pages)
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::Begin("Background", nullptr,
                         ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoBringToFrontOnFocus);

            // Shared Elements (Logo)
            ImGui::Image((void *)(intptr_t)logoTexture, ImVec2(150, 150));

            // Text Helper Lambda
            auto CenterText = [](const char *text)
            {
                float windowWidth = ImGui::GetWindowSize().x;
                float textWidth = ImGui::CalcTextSize(text).x;
                ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
                ImGui::Text(text);
            };

            switch (currentPage)
            {
            case AppState::How_to_Play:
            {

                // --- Instructions ---
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.18f);

                ImGui::SetWindowFontScale(8.5f);
                CenterText("3D GRAPHIC DEVELOPMENT");
                ImGui::Dummy(ImVec2(0.0f, 40.0f));

                ImGui::SetWindowFontScale(3.5f);
                CenterText("GUIDE");
                ImGui::Dummy(ImVec2(0.0f, 18.0f));

                ImGui::SetWindowFontScale(2.1f);
                const std::array<std::pair<const char*, const char*>, 8> guideRows = {{
                    {"Move", "W A S D"},
                    {"Sprint", "Left Shift (Hold)"},
                    {"Camera Look", "Mouse Move"},
                    {"Normal Attack", "Right Mouse Button"},
                    {"Normal Skill", "E"},
                    {"Ultimate SKill", "Q"},
                    {"Toggle Fullscreen", "F11"},
                    {"Quit Game", "ESC"},
                }};

                float keyWidthMax = 0.0f;
                float actionWidthMax = 0.0f;
                for (const auto& row : guideRows)
                {
                    keyWidthMax = std::max(keyWidthMax, ImGui::CalcTextSize(row.first).x);
                    actionWidthMax = std::max(actionWidthMax, ImGui::CalcTextSize(row.second).x);
                }

                const float colonWidth = ImGui::CalcTextSize(":").x;
                const float colGap = 18.0f;
                const float blockWidth = keyWidthMax + colGap + colonWidth + colGap + actionWidthMax;
                const float startX = (ImGui::GetWindowSize().x - blockWidth) * 0.5f;

                for (const auto& row : guideRows)
                {
                    const float keyWidth = ImGui::CalcTextSize(row.first).x;
                    ImGui::SetCursorPosX(startX + (keyWidthMax - keyWidth));
                    ImGui::TextUnformatted(row.first);

                    ImGui::SameLine(startX + keyWidthMax + colGap);
                    ImGui::TextUnformatted(":");

                    ImGui::SameLine(startX + keyWidthMax + colGap + colonWidth + colGap);
                    ImGui::TextUnformatted(row.second);
                }

                // --- Bottom "Home" Button ---
                ImGui::Dummy(ImVec2(0.0f, 40.0f));
                // Center the button
                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 500.0f) * 0.5f);
                if (ImGui::Button("Back to Home", ImVec2(500, 50)))
                {
                    currentPage = AppState::Home;
                }
                break;
            }

            case AppState::Home:

                // --- Main Menu Content ---
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.25f);
                ImGui::SetWindowFontScale(8.5f);
                CenterText("3D GRAPHIC DEVELOPMENT");

                // --- Button Layout ---
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.75f);
                float btnWidth = 200.0f;
                float centerX = (ImGui::GetWindowSize().x - btnWidth) * 0.5f;

                ImGui::SetCursorPosX(centerX);

                // Push Button Styles locally (Count: 2)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

                ImGui::SetWindowFontScale(5.5f);
                if (ImGui::Button("START", ImVec2(btnWidth, 70)))
                {
                    gameStarted = true;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }

                // Pop Button Styles immediately so they don't mess up the stack
                ImGui::PopStyleColor(2);

                // Navigation Button

                ImGui::SetWindowFontScale(5.5f);
                ImGui::SetCursorPosX(centerX);
                if (ImGui::Button("GUIDE", ImVec2(btnWidth, 70)))
                {
                    currentPage = AppState::How_to_Play;
                }
                break;
            }

            ImGui::End();

            // Pop the Global Window Styles (Matches the top 2 pushes)
            ImGui::PopStyleColor(2);
        }else{
            processInput(window);
            player.Update(window, camera, Config::deltaTime, terrainSampler);
            cameraRig.Update(camera, player.GetPosition(), terrainSampler);

            glClearColor(fogColor.r, fogColor.g, fogColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            int framebufferWidth = static_cast<int>(SCR_WIDTH);
            int framebufferHeight = static_cast<int>(SCR_HEIGHT);
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            const float aspect = (framebufferHeight > 0)
                                     ? static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight)
                                     : static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);

            const glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect, kNearPlane, kFarPlane);
            const glm::mat4 view = camera.GetViewMatrix();

            terrainShader.use();
            glActiveTexture(GL_TEXTURE10);
            glBindTexture(GL_TEXTURE_2D, sandTex);
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_2D, grassTex);
            glActiveTexture(GL_TEXTURE12);
            glBindTexture(GL_TEXTURE_2D, groundTex);
            glActiveTexture(GL_TEXTURE13);
            glBindTexture(GL_TEXTURE_2D, snowTex);

            terrainShader.setVec3("material_diffuse", 1.0f, 1.0f, 1.0f);
            terrainShader.setFloat("material_alpha", 1.0f);
            terrainShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            terrainShader.setVec3("lightPos", 100.0f, 100.0f, 400.0f);
            terrainShader.setVec3("viewPos", camera.Position);
            terrainShader.setFloat("fogDensity", 0.0004f);
            terrainShader.setVec3("fogColor", fogColor);
            terrainShader.setMat4("projection", projection);
            terrainShader.setMat4("view", view);

            glm::mat4 terrainMatrix(1.0f);
            terrainMatrix = glm::translate(terrainMatrix, kTerrainTranslate);
            terrainMatrix = glm::scale(terrainMatrix, kTerrainScale);
            terrainShader.setMat4("model", terrainMatrix);
            terrainModel.Draw(terrainShader);

            if (Model *playerFrame = player.GetCurrentFrame())
            {
                playerShader.use();
                playerShader.setMat4("projection", projection);
                playerShader.setMat4("view", view);
                playerShader.setVec3("material_diffuse", 0.9f, 0.9f, 0.9f);
                playerShader.setFloat("material_alpha", 1.0f);
                playerShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
                playerShader.setVec3("lightPos", 100.0f, 100.0f, 400.0f);
                playerShader.setVec3("viewPos", camera.Position);
                playerShader.setFloat("fogDensity", 0.0007f);
                playerShader.setVec3("fogColor", fogColor);

                glm::mat4 playerMatrix(1.0f);
                playerMatrix = glm::translate(playerMatrix, player.GetPosition());
                playerMatrix = glm::rotate(playerMatrix, glm::radians(player.GetYawDeg()), glm::vec3(0.0f, 1.0f, 0.0f));
                playerMatrix = glm::scale(playerMatrix, glm::vec3(player.GetModelScale()));
                playerShader.setMat4("model", playerMatrix);

                playerFrame->Draw(playerShader);
            }

            skybox.Draw(view, projection);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;

        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
