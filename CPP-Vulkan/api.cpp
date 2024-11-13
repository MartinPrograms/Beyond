//
// Created by marti on 12/11/2024.
//

#include "api.h"

#include <GLFW/glfw3.h>
#include <chrono>

#include "Window.h"

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

    while (!window.shouldClose()) {
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

void * CreateGraphics(void *windowptr) {
    graphics = new Graphics::VulkanBackend(reinterpret_cast<Window*>(windowptr));
    return graphics;
}
