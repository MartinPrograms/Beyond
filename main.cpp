#include <iostream>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h" // For memory allocation, because that's a pain in vulkan

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "Window.h"
#include "Graphics/VulkanBackend.h"

int main()
{
    std::cout << "Beyond engine!" << std::endl;

    auto window = Window();
    window.disableVSync();

    window.create(800, 600, "Beyond Engine");

    window.show();

    auto backend = Graphics::VulkanBackend(&window);
    backend.debug(); // This prints out some gpu info, and enables validation layers
    backend.init(); // This initializes vulkan

    // delta time calculation
    auto lastTime = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;

    Graphics::Camera camera;
    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
    camera.yaw = -90.0f;
    camera.pitch = 0.0f;
    camera.fov = 45.0f;
    camera.near = 0.1f;
    camera.far = 100.0f;

    camera.updateAspectRatio(window.width, window.height);
    camera.update();

    while (!window.shouldClose()) { // yay it works

        backend.update(deltaTime);
        backend.render(camera);
        ImGui::EndFrame(); // for a resize bug
        window.update();

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime);

        deltaTime = duration.count();

        lastTime = currentTime;

        // If W, A, S or D is pressed, move the camera
        if (glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.position += 0.01f * camera.front;
        }
        if (glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.position -= 0.01f * camera.right;
        }
        if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.position -= 0.01f * camera.front;
        }
        if (glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.position += 0.01f * camera.right;
        }
        if (glfwGetKey(window.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.position -= 0.01f * camera.up;
        }
        if (glfwGetKey(window.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.position += 0.01f * camera.up;
        }

        if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            window.close();
        }

        // camera rotation (mouse)
        if (glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window.window, &xpos, &ypos);

            static double lastX = xpos, lastY = ypos;
            double xoffset = xpos - lastX;
            double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

            lastX = xpos;
            lastY = ypos;

            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            camera.yaw += xoffset;
            camera.pitch -= yoffset;

            if (camera.pitch > 89.0f) {
                camera.pitch = 89.0f;
            }
            if (camera.pitch < -89.0f) {
                camera.pitch = -89.0f;
            }
        }

        camera.update();
    }

    return 0;
}
