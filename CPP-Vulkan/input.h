//
// Created by marti on 16/11/2024.
//

#ifndef INPUT_H
#define INPUT_H

#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

#include "Window.h"

GLFWwindow* glfwWindow;
// This is a callback function that is called when a key is pressed.
using KeyCallback = void(*)(int key, int scancode, int action, int mods);
using MouseCallback = void(*)(double xpos, double ypos);
using MouseButtonCallback = void(*)(int button, int action, int mods);

static KeyCallback keyCallback = nullptr;
static MouseCallback mouseCallback = nullptr;
static MouseButtonCallback mouseButtonCallback = nullptr;

extern "C"{

// This manages the input functions, for example, key presses, mouse movement, etc. It is static, so it can be accessed from anywhere.
API_EXPORT void InitializeInput(void* window);

API_EXPORT void SetKeyCallback(KeyCallback callback);
API_EXPORT void SetMouseCallback(MouseCallback callback);
API_EXPORT void SetMouseButtonCallback(MouseButtonCallback callback);
    API_EXPORT void SetMouseMode(int mode);
}
#endif //INPUT_H
