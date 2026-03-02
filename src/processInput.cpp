#include <processInput.h>

void processInput(GLFWwindow *window)
{
    static bool f11Pressed = false;
    static bool f8Pressed  = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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

    if(glfwGetKey(window, GLFW_KEY_F8) == GLFW_PRESS && !f8Pressed){
        static bool wireFrame = false;
        wireFrame = !wireFrame;

        if(wireFrame){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            f8Pressed = true;
        }else{
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            f8Pressed = false;
        }
        f8Pressed = true;
    }else{
        f8Pressed = false;
    }
}
