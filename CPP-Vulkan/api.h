﻿//
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
Graphics::Camera camera;

extern "C"{
    API_EXPORT void* CreateWindow(const char* name, int width, int height, bool vsync, bool fullScreen);
    API_EXPORT void DestroyWindow();

    API_EXPORT void SetRenderCallback(Callback callback);
    API_EXPORT void SetUpdateCallback(Callback callback);
    API_EXPORT void Run();

    API_EXPORT float GetDeltaTime();

    API_EXPORT void* CreateGraphics();
    API_EXPORT void UpdateGraphics();
    API_EXPORT void RenderGraphics();
    API_EXPORT void DestroyGraphics();

    API_EXPORT void SetClearColor(float r, float g, float b, float a);

    // Pipeline
    API_EXPORT void CreatePipeline(const char* name, const char* vertexShaderPath, const char* fragmentShaderPath);
    API_EXPORT void DestroyPipeline(const char* name);

    // Meshes
    API_EXPORT void* LoadMesh(const char* filePath, int index, const char* pipelineName);
    API_EXPORT void DestroyMesh(void* mesh);

    API_EXPORT Graphics::Other::Transform* GetTransform(void* mesh);
    API_EXPORT void SetTransform(void* mesh, Graphics::Other::Transform* transform);
}

#endif //API_H
