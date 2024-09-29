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

void Window::show() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    this->window = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(this->window);

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
