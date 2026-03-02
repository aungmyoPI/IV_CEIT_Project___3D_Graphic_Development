#include <init.h>

GLFWwindow* init()
{
    if(!glfwInit()){
        cerr_t("Failed to Initialize GLFW");
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Graphic Development", NULL, NULL);
    if (window == nullptr)
    {
        cerr_t("Failed to create GLFW window");
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr_t("Failed to initialize GLAD");
        return nullptr;
    }

    return window;
}

void cerr_t(const char *what)
{
    std::cerr << "[ERROR]: " << what << std::endl;
}
void cout_t(const char *what)
{
    std::cout << what << std::endl;
}
