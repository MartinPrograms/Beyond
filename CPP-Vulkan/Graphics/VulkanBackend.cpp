//
// Created by marti on 23/09/2024.
//

#include "VulkanBackend.h"


#include <array>
#include <utility>
#include <vk_mem_alloc.h>


#include "ShaderManager.h"
#include "vkinit.h"
#include "vkutil.h"

#include "glm/glm.hpp"
#include "Vulkan/MeshUtils.h"
#include "Vulkan/PipelineBuilder.h"

namespace Graphics {
    VulkanBackend::VulkanBackend(Window *window): imguiBackend(nullptr) {
        this->context = *Vulkan::VulkanContext::getInstance(); // This is a singleton, so it should be fine
        this->imguiBackend = Vulkan::ImGuiBackend(&this->context);

        this->context.window = window;
        this->defaultCamera = Camera();
        this->currentCamera = &defaultCamera;
    }

    VulkanBackend::~VulkanBackend() {
        if (!initialized) {
            return;
        }
        vkDeviceWaitIdle(context.device);

        context.mainDeletionQueue.flush();

        destroy_swapchain();

        vkDestroySurfaceKHR(context.instance, context.surface, nullptr);
        vkDestroyDevice(context.device, nullptr);

        vkb::destroy_debug_utils_messenger(context.instance,context.debugMessenger, nullptr);
        vkDestroyInstance(context.instance, nullptr);

        std::cout << "Vulkan destroyed!!" << std::endl;
    }

    void VulkanBackend::update(float deltaTime) {
        for (auto &updatable : updatables) {
            updatable(deltaTime); // This *technically* should be able to run in a separate thread, as long as it doesn't touch the GPU
        }
    }

    void VulkanBackend::recreate_swapchain() {
        vkQueueWaitIdle(context.graphicsQueue);

        vkb::SwapchainBuilder swapchainBuilder {context.physicalDevice, context.device, context.surface};

        swapchainBuilder.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        swapchainBuilder.add_image_usage_flags(VK_IMAGE_USAGE_SAMPLED_BIT);
        swapchainBuilder.add_image_usage_flags(VK_IMAGE_USAGE_STORAGE_BIT);

        unsigned int width, height;
        context.window->getFramebufferSize(&width, &height);

        // Destroy the old image views
        for (auto imageView : swapchainImageViews) {
            vkDestroyImageView(context.device, imageView, nullptr);
        }

        vkDestroyImageView(context.device, _drawImage.imageView, nullptr);
        vkDestroyImageView(context.device, _depthImage.imageView, nullptr); // Destroy the depth image view

        swapchainImageViews.clear();

        vkDestroySwapchainKHR(context.device, context.swapchain, nullptr);
        vmaDestroyImage(context.allocator, _drawImage.image, _drawImage.allocation);
        vmaDestroyImage(context.allocator, _depthImage.image, _depthImage.allocation);

        // width, height hdr
        create_swapchain(width, height, false);


        VkExtent3D imageExtent = { .width = context.swapchainExtent.width, .height = context.swapchainExtent.height, .depth = 1 };

        _drawImage.imageFormat = context.swapchainImageFormat;
        _depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;

        VkImageUsageFlags usage {};
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;

        // DISABLE VK_IMAGE_TILING_OPTIMAL
        VkImageCreateInfo imageInfo = vkinit::image_create_info(_drawImage.imageFormat, usage, imageExtent);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vmaCreateImage(context.allocator, &imageInfo, &allocInfo, &_drawImage.image, &_drawImage.allocation, nullptr);

        VkImageViewCreateInfo viewInfo = vkinit::image_view_create_info(_drawImage.imageFormat, _drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

        vkutil::VK_CHECK(vkCreateImageView(context.device, &viewInfo, nullptr, &_drawImage.imageView));

        _depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
        _depthImage.imageExtent = imageExtent;
        VkImageUsageFlags depthImageUsages{};
        depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VkImageCreateInfo dimg_info = vkinit::image_create_info(_depthImage.imageFormat, depthImageUsages, imageExtent);

        //allocate and create the image
        vmaCreateImage(context.allocator, &dimg_info, &allocInfo, &_depthImage.image, &_depthImage.allocation, nullptr);

        //build a image-view for the draw image to use for rendering
        VkImageViewCreateInfo dview_info = vkinit::image_view_create_info(_depthImage.imageFormat, _depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

        vkutil::VK_CHECK(vkCreateImageView(context.device, &dview_info, nullptr, &_depthImage.imageView));

        VkDescriptorImageInfo drawImageInfo = {};
        drawImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        drawImageInfo.imageView = _drawImage.imageView;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        write.descriptorCount = 1;
        write.dstSet = drawImageDescriptorSet;
        write.dstBinding = 0;
        write.pImageInfo = &drawImageInfo;
    }


    void VulkanBackend::submitQueue(const Vulkan::FrameData &frame) {
        VkCommandBufferSubmitInfo cmdSubmitInfo = vkinit::command_buffer_submit_info(frame.mainCommandBuffer);

        VkSemaphoreSubmitInfo signalSemaphore = vkinit::semaphore_submit_info(frame.renderFinishedSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        VkSemaphoreSubmitInfo waitSemaphore = vkinit::semaphore_submit_info(frame.imageAvailableSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);;

        auto submit_info = vkinit::submit_info(&cmdSubmitInfo, &signalSemaphore, &waitSemaphore);

        vkQueueSubmit2(context.graphicsQueue, 1, &submit_info, frame.renderFence);
    }

    void VulkanBackend::queuePresent(Vulkan::FrameData frame, uint32_t imageIndex) {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &frame.renderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &context.swapchain;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(context.graphicsQueue, &presentInfo);
    }

    bool VulkanBackend::waitFencesAcquireImage(const Vulkan::FrameData& frame, uint32_t &imageIndex) {
        vkutil::VK_CHECK(vkWaitForFences(context.device, 1, &frame.renderFence, VK_TRUE, 5000000000)); // a second or something idfk

        VkResult result = vkAcquireNextImageKHR(context.device, context.swapchain, UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        if (result != VK_SUCCESS) {
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                recreate_swapchain();
                return true;
            } else {
                throw std::runtime_error("Failed to acquire swapchain image!");
            }
        }

        vkutil::VK_CHECK(vkResetFences(context.device, 1, &frame.renderFence)); // Reset the fence
        return false;
    }

    void VulkanBackend::render() {
        auto frame = getCurrentFrame(); // This performs a modulo operation, so it's fine

        uint32_t imageIndex;
        if (waitFencesAcquireImage(frame, imageIndex)) return;

        // Reset the command buffer
        vkResetCommandBuffer(frame.mainCommandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo = vkinit::command_buffer_begin_info();
        vkBeginCommandBuffer(frame.mainCommandBuffer, &beginInfo);

        // Now we need to transition the swapchain image to a layout that our render pass can use
        vkutil::transition_image(frame.mainCommandBuffer, swapchainImages[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        vkutil::transition_image(frame.mainCommandBuffer, _depthImage.image,  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        // Start the render pass
        VkClearValue clearColor = { .color = {this->clearColor.r, this->clearColor.g, this->clearColor.b, this->clearColor.a} };

        VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(swapchainImageViews[imageIndex], &clearColor, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VkRenderingAttachmentInfo depthAttachment = vkinit::depth_attachment_info(_depthImage.imageView, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        VkRenderingInfo renderInfo = vkinit::rendering_info(context.swapchainExtent, &colorAttachment, &depthAttachment);

        vkCmdBeginRendering(frame.mainCommandBuffer, &renderInfo);

        auto scissors = vkinit::scissor(context.swapchainExtent);
        auto viewport = vkinit::viewport(context.swapchainExtent);

        vkCmdSetScissor(frame.mainCommandBuffer, 0, 1, &scissors);
        vkCmdSetViewport(frame.mainCommandBuffer, 0, 1, &viewport);

        for (auto &drawable : drawables) {
            drawable(frame.mainCommandBuffer, _drawImage.imageView);
        }

        for (auto &drawable : drawQueue) {
            drawable(frame.mainCommandBuffer, _drawImage.imageView);
        }

        drawQueue.clear();

        imguiBackend.draw(frame.mainCommandBuffer, _drawImage.imageView);

        vkCmdEndRendering(frame.mainCommandBuffer); // that concludes in engine rendering



        vkutil::transition_image(frame.mainCommandBuffer, swapchainImages[imageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        vkEndCommandBuffer(frame.mainCommandBuffer);

        // Prepare the submission to the queue
        submitQueue(frame);

        // Now present the image
        queuePresent(frame, imageIndex);

        currentFrame++;
    }

    void VulkanBackend::debug() {
        this->debugMode = true;
        this->enableValidationLayers = true;
    }

    void VulkanBackend::init() {
        // Initialize Vulkan...

        init_vulkan();
        init_swapchain();
        init_commands();
        init_sync_structures();

        init_imgui_pipeline();

        this->initialized = true;
    }

    Graphics::Mesh::Mesh VulkanBackend::loadMesh(const char *str, int index, std::string pipelineName) {
        auto meshUtils = Vulkan::MeshUtils(context);

        std::cout << "Created mesh utils, loading mesh" << std::endl;
        auto mesh = meshUtils.loadMesh(str, index, std::move(pipelineName));
        std:: cout << "Mesh loaded!" << std::endl;

        context.mainDeletionQueue.push_function([=]() {
            mesh.destroy(context);
        });

        return mesh;
    }

    void VulkanBackend::destroyMesh(Mesh::Mesh &mesh) {
        mesh.destroy(context);
    }

    void VulkanBackend::createPipeline(const char *name, const char *vert, const char *frag) {

        if (_meshPipelineLayout == VK_NULL_HANDLE) {
            auto info = vkinit::pipeline_layout_create_info();

            VkPushConstantRange pushConstantsRange = {};
            pushConstantsRange.offset = 0;
            pushConstantsRange.size = sizeof(Vulkan::GPUDrawPushConstants);
            pushConstantsRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            info.pPushConstantRanges = &pushConstantsRange;
            info.pushConstantRangeCount = 1;

            vkutil::VK_CHECK(vkCreatePipelineLayout(context.device, &info, nullptr, &_meshPipelineLayout));
        }

        auto shaderManager = ShaderManager::getInstance();

        std::cout << "Creating pipeline " << name << std::endl;

        shaderManager->createPipeline(name, frag, vert, context.device, _meshPipelineLayout, context.swapchainImageFormat, VK_FORMAT_D32_SFLOAT, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT);
        std::cout << "Pipeline created!" << std::endl;
        auto pipeline = shaderManager->getPipeline(name);

        context.mainDeletionQueue.push_function([=]() {
            // Destroy the _meshPipelineLayout
            vkDestroyPipelineLayout(context.device, _meshPipelineLayout, nullptr);

            if (pipeline != VK_NULL_HANDLE) {
                vkDestroyPipeline(context.device, pipeline, nullptr);
                std::cout << "Destroyed pipeline!" << std::endl;

            }
        });
    }

    void VulkanBackend::destroyPipeline(const char *str) {
        auto shaderManager = ShaderManager::getInstance();

        shaderManager->destroyPipeline(str);
    }

    void VulkanBackend::renderMesh(Mesh::Mesh &mesh) {
        // Add to the drawQueue
        drawQueue.push_back([&](VkCommandBuffer cmd, VkImageView drawImage) mutable {
            mesh.draw(cmd, currentCamera, drawImage, drawImageDescriptorSet);
        });
    }

    void VulkanBackend::useCamera(Camera &camera) {

        drawQueue.push_back([&](VkCommandBuffer cmd, VkImageView drawImage) mutable {
            this->currentCamera = &camera;
        });

        this->lastSetCamera = camera;
    }


    void VulkanBackend::init_vulkan() {
        vkb::InstanceBuilder builder;

        auto extensions = Window::get_extensions();

// If macos, add the VK_KHR_surface", "VK_MVK_macos_surface" extensions
#ifdef __APPLE__
        extensions.push_back("VK_KHR_surface");
        extensions.push_back("VK_MVK_macos_surface");
#endif
        // Ok just found out macos does not even support vulkan 1.3, only 1.2. Love you tim apple

        auto b = builder.set_app_name("Beyond Engine")
            .request_validation_layers(this->enableValidationLayers);

        for (auto &extension : extensions) {
            b.enable_extension(extension.c_str());
        }

        auto output = b.use_default_debug_messenger()
            .require_api_version(1,3,0)
            .build();

        if (!output)
            std::cout << output.error().message() << std::endl;


        vkb::Instance vkb_inst = output.value();
        this->context.instance = vkb_inst.instance;
        this->context.debugMessenger = vkb_inst.debug_messenger;

        // Creating a surface
        VkSurfaceKHR vkSurface;
        if (context.window->create_surface(context.instance, &vkSurface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }

        this->context.surface = vkSurface;

        VkPhysicalDeviceVulkan13Features vulkan13Features{};
        vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vulkan13Features.pNext = nullptr;
        vulkan13Features.dynamicRendering = VK_TRUE;
        vulkan13Features.synchronization2 = VK_TRUE;
        vulkan13Features.inlineUniformBlock = VK_TRUE;

        VkPhysicalDeviceVulkan12Features vulkan12Features{};
        vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vulkan12Features.pNext = nullptr;
        vulkan12Features.bufferDeviceAddress = VK_TRUE;
        vulkan12Features.descriptorIndexing = VK_TRUE; // For descriptor indexing (this is a feature that allows you to use arrays of descriptors)

        VkPhysicalDeviceFeatures features{};
        features.samplerAnisotropy = VK_TRUE;
        features.shaderStorageImageMultisample = VK_TRUE;
        features.shaderStorageImageExtendedFormats = VK_TRUE;


        vkb::PhysicalDeviceSelector selector {vkb_inst};
        vkb::PhysicalDevice physical_device = selector.set_surface(context.surface)
            .set_required_features_13(vulkan13Features)
            .set_required_features_12(vulkan12Features)
            .set_required_features(features)
            .set_minimum_version(1,3)
            .select()
            .value();

        vkb::DeviceBuilder deviceBuilder {physical_device};
        vkb::Device vkb_device = deviceBuilder.build().value();

        this->context.physicalDevice = physical_device.physical_device;
        this->context.device = vkb_device.device;

        context.graphicsQueue = vkb_device.get_queue(vkb::QueueType::graphics).value();
        context.graphicsQueueFamily = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        std::cout << "Vulkan initialized! Chosen GPU: " << physical_device.name << std::endl;

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = context.physicalDevice;
        allocatorInfo.device = context.device;
        allocatorInfo.instance = context.instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocatorInfo, &context.allocator);

        context.mainDeletionQueue.push_function([&]() {
            vmaDestroyAllocator(context.allocator);
        });


    }

    void VulkanBackend::init_swapchain() {

        auto samples = vkutil::get_max_usable_sample_count();

        std::cout << "Max samples: " << samples << std::endl;

        create_swapchain(context.window->width, context.window->height);

        VkExtent3D imageExtent = { .width = context.swapchainExtent.width, .height = context.swapchainExtent.height, .depth = 1 };
        this->_drawImage.imageFormat = this->context.swapchainImageFormat;
        this->_drawImage.imageExtent = imageExtent;

        VkImageUsageFlags usage {};
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

        VkImageCreateInfo imageInfo = vkinit::image_create_info(this->_drawImage.imageFormat, usage, imageExtent);

        imageInfo.samples = vkutil::SampleCountFlagBits;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vmaCreateImage(context.allocator, &imageInfo, &allocInfo, &this->_drawImage.image, &this->_drawImage.allocation, nullptr);

        VkImageViewCreateInfo viewInfo = vkinit::image_view_create_info(this->_drawImage.imageFormat, this->_drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

        auto r = vkCreateImageView(context.device, &viewInfo, nullptr, &this->_drawImage.imageView);
        if (r != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view!");
        }

        // Create the depth image
        this->_depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
        this->_depthImage.imageExtent = imageExtent;
        VkImageUsageFlags depthImageUsages{};
        depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VkImageCreateInfo dimg_info = vkinit::image_create_info(_depthImage.imageFormat, depthImageUsages, imageExtent);

        //allocate and create the image
        vmaCreateImage(context.allocator, &dimg_info, &allocInfo, &_depthImage.image, &_depthImage.allocation, nullptr);

        //build a image-view for the draw image to use for rendering
        VkImageViewCreateInfo dview_info = vkinit::image_view_create_info(_depthImage.imageFormat, _depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

        vkutil::VK_CHECK(vkCreateImageView(context.device, &dview_info, nullptr, &_depthImage.imageView));


        context.mainDeletionQueue.push_function([=]() {
            vkDestroyImageView(context.device, this->_drawImage.imageView, nullptr);
            vmaDestroyImage(context.allocator, this->_drawImage.image, this->_drawImage.allocation);

            vkDestroyImageView(context.device, this->_depthImage.imageView, nullptr);
            vmaDestroyImage(context.allocator, this->_depthImage.image, this->_depthImage.allocation);
        });
    }

    void VulkanBackend::create_swapchain(unsigned int width, unsigned int height, bool hdr) {
        vkb::SwapchainBuilder swapchainBuilder {context.physicalDevice, context.device, context.surface};
        swapchainBuilder.set_desired_present_mode(context.presentMode);

        if (hdr) {
            this->context.swapchainImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            this->context.colorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT;
        }

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            //.use_default_format_selection()
            .set_desired_format(VkSurfaceFormatKHR{.format = this->context.swapchainImageFormat, .colorSpace = this->context.colorSpace})
            //use vsync present mode
            .set_desired_present_mode(this->context.presentMode)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT).add_image_usage_flags(VK_IMAGE_USAGE_SAMPLED_BIT)
            .build()
            .value();

        this->context.swapchain = vkbSwapchain.swapchain;
        this->context.swapchainExtent = vkbSwapchain.extent;
        this->swapchainImages = vkbSwapchain.get_images().value();
        this->swapchainImageViews = vkbSwapchain.get_image_views().value();

        std::cout << "Swapchain created! Image count: " << this->swapchainImages.size() << std::endl;
    }

    void VulkanBackend::destroy_swapchain() {
        vkDeviceWaitIdle(context.device);

        vkDestroySwapchainKHR(context.device, context.swapchain, nullptr);

        for (auto imageView : swapchainImageViews) {
            vkDestroyImageView(context.device, imageView, nullptr);
        }

        swapchainImageViews.clear();
        swapchainImages.clear();
    }

    void VulkanBackend::init_commands() {
        auto commandPoolInfo = vkinit::command_pool_create_info(this->context.graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        for (int i = 0; i < FRAME_OVERLAP; i++) {
            vkCreateCommandPool(context.device, &commandPoolInfo, nullptr, &frames[i].commandPool);

            VkCommandBufferAllocateInfo commandBufferInfo = vkinit::command_buffer_allocate_info(frames[i].commandPool);
            vkAllocateCommandBuffers(context.device, &commandBufferInfo, &frames[i].mainCommandBuffer);
        }

        // Immediate mode command pool
        auto r =vkCreateCommandPool(context.device, &commandPoolInfo, nullptr, &context.immediateCommandPool);

        if (r != VK_SUCCESS) {
            throw std::runtime_error("Failed to create immediate command pool!");
        }

        VkCommandBufferAllocateInfo commandBufferInfo = vkinit::command_buffer_allocate_info(context.immediateCommandPool);
        r= vkAllocateCommandBuffers(context.device, &commandBufferInfo, &context.immediateCommandBuffer);

        if (r != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate immediate command buffer!");
        }

        context.mainDeletionQueue.push_function([=]() {
            for (int i = 0; i < FRAME_OVERLAP; i++) {
                vkDestroyCommandPool(context.device, frames[i].commandPool, nullptr);
            }

            vkDestroyCommandPool(context.device, context.immediateCommandPool, nullptr);
        });
    }

    void VulkanBackend::init_sync_structures() {
        for (int i = 0; i < FRAME_OVERLAP; i++) {
            VkSemaphoreCreateInfo semaphoreInfo = vkinit::semaphore_create_info();
            vkCreateSemaphore(context.device, &semaphoreInfo, nullptr, &frames[i].imageAvailableSemaphore);
            vkCreateSemaphore(context.device, &semaphoreInfo, nullptr, &frames[i].renderFinishedSemaphore);

            VkFenceCreateInfo fenceInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
            vkCreateFence(context.device, &fenceInfo, nullptr, &frames[i].renderFence);
        }

        auto fenceInfo = vkinit::fence_create_info(0);
        auto r = vkCreateFence(context.device, &fenceInfo, nullptr, &context.immediateFence);
        vkutil::VK_CHECK(r);

        context.mainDeletionQueue.push_function([=]() {
            for (int i = 0; i < FRAME_OVERLAP; i++) {
                vkDestroySemaphore(context.device, frames[i].imageAvailableSemaphore, nullptr);
                vkDestroySemaphore(context.device, frames[i].renderFinishedSemaphore, nullptr);
                vkDestroyFence(context.device, frames[i].renderFence, nullptr);
            }

            vkDestroyFence(context.device, context.immediateFence, nullptr);
        });
    }

    void VulkanBackend::init_imgui_pipeline() {
        this->imguiBackend.init();

        this->updatables.push_back([&](float deltaTime) {
            imguiBackend.update(deltaTime);
        });
    }
} // Graphics