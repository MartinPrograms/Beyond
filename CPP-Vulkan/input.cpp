#include "input.h"

void InitializeInput(void* window) {
    // Static cast to Window* because the window is a void pointer
    glfwWindow = static_cast<Window*>(window)->window;

    // Register the key callback
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (keyCallback != nullptr)
            keyCallback(key, scancode, action, mods);
    });

    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
        if (mouseCallback != nullptr)
            mouseCallback(xpos, ypos);
    });

    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int button, int action, int mods) {
        if (mouseButtonCallback != nullptr)
            mouseButtonCallback(button, action, mods);
    });
}

void SetKeyCallback(KeyCallback callback) {
    keyCallback = callback;
}

void SetMouseCallback(MouseCallback callback) {
    mouseCallback = callback;
}

void SetMouseButtonCallback(MouseButtonCallback callback) {
    mouseButtonCallback = callback;
}

void SetMouseMode(int mode) {
    // 0 = normal, 1 = hidden, 2 = disabled
    if (mode == 0)
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else if (mode == 1)
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    else if (mode == 2)
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
