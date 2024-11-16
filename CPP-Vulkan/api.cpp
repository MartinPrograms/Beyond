//
// Created by marti on 12/11/2024.
//

#include "api.h"

#include <GLFW/glfw3.h>
#include <chrono>

#include "Window.h"
#include "Graphics/ShaderManager.h"

// is c++ code

void DestroyWindow() {
    window.close();
}

void SetRenderCallback(Callback callback) {
    renderCallback = callback;
}

void SetUpdateCallback(Callback callback) {
    updateCallback = callback;
}

void Run() {
    auto lastTime = std::chrono::high_resolution_clock::now();

    std::cout << "Running" << std::endl;

    bool first_run = true;

    while (!window.shouldClose()) {

        if (first_run) {
            if (loadCallback != nullptr)
                loadCallback(); // This is for loading stuff, like meshes, textures, etc. Can be done later yes, but this is for things that need to be loaded before the main loop starts
            first_run = false;
        }

        if (updateCallback != nullptr)
            updateCallback();

        if (renderCallback != nullptr)
            renderCallback();

        window.update();

        auto currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;
    }
}

float GetDeltaTime() {
    return deltaTime;
}

void * CreateWindow(const char *name, int width, int height, bool vsync, bool fullScreen) {
    window = Window();

    if (!vsync)
        window.disableVSync();

    if (fullScreen) {
        window.fullScreen();
    }

    window.create(width, height, name);

    window.show();

    return &window;
}

void * CreateGraphics() {
    graphics = new Graphics::VulkanBackend(&window);
    graphics->debug(); // This prints out some gpu info, and enables validation layers
    graphics->init(); // This initializes vulkan

    return graphics;
}

void UpdateGraphics() {
    graphics->update(deltaTime);
}

void RenderGraphics() {
    graphics->render();
}

void DestroyGraphics() {
    graphics->~VulkanBackend();
}

void CreatePipeline(const char *name, const char *vertexShaderPath, const char *fragmentShaderPath) {
    graphics->createPipeline(name, vertexShaderPath, fragmentShaderPath);
}

void DestroyPipeline(const char *name) {
    graphics->destroyPipeline(name);
}

void SetClearColor(float r, float g, float b, float a) {
    graphics->clearColor = {r, g, b, a};
}

void* LoadMesh(const char* filePath, int index, const char* pipelineName) {
    auto* mesh = new Graphics::Mesh::Mesh(graphics->loadMesh(filePath, index, pipelineName));
    return static_cast<void*>(mesh);
}

void DestroyMesh(void *mesh) {
    graphics->destroyMesh(*static_cast<Graphics::Mesh::Mesh *>(mesh));
    delete static_cast<Graphics::Mesh::Mesh *>(mesh);
}

void SetLoadCallback(Callback callback) {
    loadCallback = callback;
}

Graphics::Other::Transform * GetTransform(void *mesh) {
    return &static_cast<Graphics::Mesh::Mesh *>(mesh)->transform;
}

Graphics::Other::Transform * CreateTransform() {
    return new Graphics::Other::Transform();
}

void SetTransform(void *mesh, Graphics::Other::Transform *transform) {
    static_cast<Graphics::Mesh::Mesh *>(mesh)->transform = *transform;
}

void RenderMesh(void *mesh) {
    graphics->renderMesh(*static_cast<Graphics::Mesh::Mesh *>(mesh));
}
