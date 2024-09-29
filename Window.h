//
// Created by marti on 23/09/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

class Window {
public:
    unsigned int width;
    unsigned int height;
    std::string title;
    GLFWwindow* window;

    [[nodiscard]] bool shouldClose() const;

    void create(unsigned int width, unsigned int height, const std::string &title);
    void update();
    void close() const;
    void show();
    void disableVSync() const;

    VkResult create_surface(VkInstance instance, VkSurfaceKHR* surface) const;

    void getFramebufferSize(unsigned int * width, unsigned int * height);

private:
    bool shouldCloseFlag = false;
};



#endif //WINDOW_H
