//
// Created by marti on 24/09/2024.
//

#include "ImGuiBackend.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "../vkinit.h"

namespace Graphics::Vulkan {
    ImGuiBackend::ImGuiBackend(VulkanContext *backend) {
        this->backend = backend;
    }

    ImGuiBackend::~ImGuiBackend() {
        // Do nothing (i think)
    }

    void ImGuiBackend::init() {

        VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        VkDescriptorPool descriptorPool;
        vkutil::VK_CHECK(vkCreateDescriptorPool(backend->device, &pool_info, nullptr, &descriptorPool));

        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForVulkan(backend->window->window, true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = backend->instance;
        init_info.PhysicalDevice = backend->physicalDevice;
        init_info.Device = backend->device;
        init_info.Queue = backend->graphicsQueue;
        init_info.DescriptorPool = descriptorPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = vkutil::check_vk_result;
        init_info.UseDynamicRendering = true; // makes it so much easier to use

        init_info.PipelineRenderingCreateInfo = {};
        init_info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &backend->swapchainImageFormat;

        ImGui_ImplVulkan_Init(&init_info);

        ImGui_ImplVulkan_CreateFontsTexture();

        backend->mainDeletionQueue.push_function([=]() {
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            vkDestroyDescriptorPool(backend->device, descriptorPool, nullptr); // if we do this before the imgui stuff, it crashes
        });
    }

    void ImGuiBackend::update(float dt) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


    }

    // It has already begun the render pass, we just add the imgui draw data
    void ImGuiBackend::draw(VkCommandBuffer cb, VkImageView view) {

        ImGui::Render();

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb);

    }
}
