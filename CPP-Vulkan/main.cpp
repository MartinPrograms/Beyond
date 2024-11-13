#include <iostream>

#include "Window.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/VulkanBackend.h"
#include "Graphics/Vulkan/PipelineBuilder.h"

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

    Graphics::ShaderManager shaderManager = Graphics::ShaderManager(); // to initialize the shader manager

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

    backend.createPipeline("test", "Shaders/default.frag.spv", "Shaders/default.vert.spv");

    auto mesh = backend.loadMesh("Models/cube.obj", 0, "test"); // Load a cube mesh

    backend.drawables.push_back([&](VkCommandBuffer cmd, VkImageView swapchainImageView) {
        mesh.draw(cmd, camera, swapchainImageView, backend.render_descriptor_set);
    });

    // this function has a float parameter, which is the delta time
    backend.updatables.push_back([&](float deltaTime) {

    });

    auto previousMouseState = GLFW_RELEASE;
    double lastX = window.width / 2;
    double lastY = window.height / 2;

    while (!window.shouldClose()) { // yay it works
        backend.update(deltaTime);

        ImGui::Begin("Beyond");
        // round fps to 3 decimal places
        ImGui::Text("FPS: %.3f", 1.0f / deltaTime);
        ImGui::Text("Frame time: %.3f ms", deltaTime * 1000.0f);
        ImGui::End();

        backend.render(camera);
        ImGui::EndFrame(); // for a resize bug
        window.update();

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime);

        deltaTime = duration.count();

        lastTime = currentTime;

        float speed = 2.5f * deltaTime;

        // If W, A, S or D is pressed, move the camera
        if (glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.position += speed * camera.front;
        }
        if (glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.position -= speed * camera.right;
        }
        if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.position -= speed * camera.front;
        }
        if (glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.position += speed * camera.right;
        }
        if (glfwGetKey(window.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.position -= speed * camera.up;
        }
        if (glfwGetKey(window.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.position += speed * camera.up;
        }

        if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            window.close();
        }

        // camera rotation (mouse)
        if (glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && previousMouseState == GLFW_RELEASE) {
            previousMouseState = GLFW_PRESS;
            // Set the cursor mode to raw
            glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(window.window, window.width / 2, window.height / 2);

            lastX = window.width / 2;
            lastY = window.height / 2;
        }
        else
        if (glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {

            previousMouseState = GLFW_PRESS;
            // Set the cursor mode to raw
            glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


            double xpos, ypos;
            glfwGetCursorPos(window.window, &xpos, &ypos);

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
        else {
            if (previousMouseState == GLFW_PRESS) {
                previousMouseState = GLFW_RELEASE;
                glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }

        camera.update();
        camera.updateAspectRatio(window.width, window.height);
    }

    return 0;
}