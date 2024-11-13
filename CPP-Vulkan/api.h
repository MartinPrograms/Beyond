//
// Created by marti on 12/11/2024.
//

#ifndef API_H
#define API_H

#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif
#include "Window.h"
#include "Graphics/VulkanBackend.h"

using Callback = void(*)();

static Callback renderCallback = nullptr;
static Callback updateCallback = nullptr;

float deltaTime = 0.0f;

Window window;
Graphics::VulkanBackend* graphics;

extern "C"{
    API_EXPORT void* CreateWindow(const char* name, int width, int height, bool vsync, bool fullScreen);
    API_EXPORT void DestroyWindow();

    API_EXPORT void SetRenderCallback(Callback callback);
    API_EXPORT void SetUpdateCallback(Callback callback);
    API_EXPORT void Run();

    API_EXPORT float GetDeltaTime();

    API_EXPORT void* CreateGraphics(void* windowptr);
}

#endif //API_H
