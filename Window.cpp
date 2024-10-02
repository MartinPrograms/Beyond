//
// Created by marti on 23/09/2024.
//

#include "Window.h"

bool Window::shouldClose() const {
    return this->shouldCloseFlag;
}

auto Window::create(const unsigned int width, const unsigned int height, const std::string &title) -> void {
    this->width = width;
    this->height = height;
    this->title = title;
}

void Window::update() {
    this->shouldCloseFlag = glfwWindowShouldClose(this->window);
    glfwPollEvents();
    glfwSwapBuffers(this->window);
}

void Window::close() const {
    glfwSetWindowShouldClose(this->window, true);
}
void error_callback(int error, const char* description)
{
    std::cout << stderr << "Error: " << description << std::endl;
}
void Window::show() {
    glfwSetErrorCallback(error_callback);
    if(glfwPlatformSupported(GLFW_PLATFORM_WIN32)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
    else if(glfwPlatformSupported(GLFW_PLATFORM_COCOA)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_COCOA);
    else if(glfwPlatformSupported(GLFW_PLATFORM_X11)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    else if(glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    else {
        fprintf(stderr, "Error: could not find acceptable platform for GLFW\n");
        abort();
    }

    if (!glfwInit())
        throw std::runtime_error("GLFW initialization failed!");
    if (!glfwVulkanSupported())
        throw std::runtime_error("GLFW reports to have no Vulkan support! Maybe it couldn't "
                                 "find the Vulkan loader!");


    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    this->window = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);

    // The update loop will not be in this class, it is just for the window creation and destruction
    // The update loop will be in the main.cpp file

    glfwSetWindowUserPointer(this->window, this);
}

void Window::disableVSync() const {
    glfwSwapInterval(0);
}

VkResult Window::create_surface(const VkInstance instance, VkSurfaceKHR*surface) const {

    return glfwCreateWindowSurface(instance, this->window, nullptr, surface);
}

void Window::getFramebufferSize(unsigned int *width, unsigned int *height) {
    return glfwGetFramebufferSize(this->window, (int*)width, (int*)height);
}

std::vector<std::string> Window::get_extensions() {

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        std::string extension = glfwExtensions[i];
        std::cout << extension << std::endl;
    }

    std::vector<std::string> extensions;
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }

    return extensions;
}
