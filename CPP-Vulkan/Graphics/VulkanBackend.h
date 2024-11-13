//
// Created by marti on 23/09/2024.
//

#ifndef VULKANBACKEND_H
#define VULKANBACKEND_H

#include <deque>
#include <functional>
#include <glm/vec4.hpp>

#include "../Window.h"

#include <KHR/khrplatform.h>
#include <vulkan/vulkan.h>

#include "Camera.h"
#include "Mesh.h"
#include "VkBootstrap.h" // love this library :3 xD

#include "Vulkan/Structs.h"
#include "Vulkan/ImGuiBackend.h"

#include "vk_mem_alloc.h" // For memory allocation, because that's a pain in vulkan
#include "Vulkan/DescriptorAllocator.h"
#include "Vulkan/VulkanContext.h"

constexpr int FRAME_OVERLAP = 2;

namespace Graphics {
    class VulkanBackend {
    public:

        glm::vec4 clearColor = {0.2f, 0.3f, 0.4f, 1.0f};

        // Devices & instances
        Vulkan::VulkanContext context; // Contains the vulkan context, and everything related to it
        std::vector<VkImage> swapchainImages; // The images in the swapchain
        std::vector<VkImageView> swapchainImageViews;
        Graphics::Vulkan::ImGuiBackend imguiBackend;

        // drawables is a list of things to draw.
        std::deque<std::function<void(VkCommandBuffer, VkImageView)>> drawables;
        std::deque<std::function<void(float deltaTime)>> updatables;
        VkDescriptorSet render_descriptor_set;

        explicit VulkanBackend(Window* window);
        ~VulkanBackend();

        void update(float deltaTime);

        void recreate_swapchain();

        void submitQueue(const Vulkan::FrameData &frame);

        void queuePresent(Vulkan::FrameData frame, uint32_t imageIndex);

        bool waitFencesAcquireImage(const Vulkan::FrameData& frame, uint32_t &imageIndex);

        void render();
        void debug();
        void init();

        void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

        Vulkan::GPUMeshBuffers upload_mesh(std::span<uint32_t> indices, std::span<Vulkan::Vertex> vertices);

        Graphics::Mesh::Mesh loadMesh(const char *str, int index, std::string pipelineName);

        void createPipeline(const char * str, const char * text, const char * string);

    private:
        bool initialized = false;

        bool enableValidationLayers = false;
        bool debugMode = false; // Print out some stuff


        // Swapchain

        Vulkan::AllocatedImage _drawImage;

        Vulkan::FrameData frames[FRAME_OVERLAP];
        int currentFrame = 0;
        Vulkan::FrameData& getCurrentFrame(){ return frames[currentFrame % FRAME_OVERLAP]; }


        DescriptorAllocator descriptorAllocator;
        VkDescriptorSet drawImageDescriptorSet;
        VkDescriptorSetLayout drawImageDescriptorSetLayout;

        void init_vulkan();

        void init_swapchain();
        void create_swapchain(unsigned int width, unsigned int height, bool hdr = false);
        void destroy_swapchain();

        void init_commands();
        void init_sync_structures();
        void init_imgui_pipeline();

        VkPipelineLayout _meshPipelineLayout = VK_NULL_HANDLE;
        VkPipeline _meshPipeline;

        void draw_geometry(VkCommandBuffer cmd, Camera &camera, VkPipeline pipeline);
    };

} // Graphics

#endif //VULKANBACKEND_H
