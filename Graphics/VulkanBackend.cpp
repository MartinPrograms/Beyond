//
// Created by marti on 23/09/2024.
//

#include "VulkanBackend.h"


#include <array>
#include <vk_mem_alloc.h>


#include "ShaderManager.h"
#include "vkinit.h"
#include "vkutil.h"

#include "glm/glm.hpp"
#include "Vulkan/PipelineBuilder.h"

namespace Graphics {
    VulkanBackend::VulkanBackend(Window *window): imguiBackend(nullptr) {
        this->context = Vulkan::VulkanContext();
        this->imguiBackend = Vulkan::ImGuiBackend(&this->context);
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

        std::cout << "Vulkan destroyed! Don't forget to cleanup the window as well!" << std::endl;
    }

    void VulkanBackend::update(float deltaTime) {
        for (auto &updatable : updatables) {
            updatable(deltaTime); // This *technically* should be able to run in a separate thread, as long as it doesn't touch the GPU
        }
    }

    void VulkanBackend::recreate_swapchain() {
        vkQueueWaitIdle(context.graphicsQueue);

        vkb::SwapchainBuilder swapchainBuilder {context.physicalDevice, context.device, context.surface};

        unsigned int width, height;
        context.window->getFramebufferSize(&width, &height);

        vkDestroySwapchainKHR(context.device, context.swapchain, nullptr);
        vkDestroyImageView(context.device, _drawImage.imageView, nullptr);
        vmaDestroyImage(context.allocator, _drawImage.image, _drawImage.allocation);

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            //.use_default_format_selection()
            .set_desired_format(VkSurfaceFormatKHR{ .format = this->context.swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            //use vsync present mode
            .set_desired_present_mode(context.presentMode ) // no vsync
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

        context.swapchain = vkbSwapchain.swapchain;
        context.swapchainExtent = vkbSwapchain.extent;
        context.swapchainImageFormat = vkbSwapchain.image_format;

        swapchainImages = vkbSwapchain.get_images().value();
        swapchainImageViews = vkbSwapchain.get_image_views().value();

        VkExtent3D imageExtent = { .width = context.swapchainExtent.width, .height = context.swapchainExtent.height, .depth = 1 };

        _drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

        VkImageUsageFlags usage {};
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

        VkImageCreateInfo imageInfo = vkinit::image_create_info(_drawImage.imageFormat, usage, imageExtent);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vmaCreateImage(context.allocator, &imageInfo, &allocInfo, &_drawImage.image, &_drawImage.allocation, nullptr);

        VkImageViewCreateInfo viewInfo = vkinit::image_view_create_info(_drawImage.imageFormat, _drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

        vkutil::VK_CHECK(vkCreateImageView(context.device, &viewInfo, nullptr, &_drawImage.imageView));

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
        vkutil::VK_CHECK(vkWaitForFences(context.device, 1, &frame.renderFence, VK_TRUE, 5000000000)); // a second

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

    void VulkanBackend::render(Camera &camera) {
        auto frame = getCurrentFrame(); // This performs a modulo operation, so it's fine

        uint32_t imageIndex;
        if (waitFencesAcquireImage(frame, imageIndex)) return;

        // Reset the command buffer
        vkResetCommandBuffer(frame.mainCommandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo = vkinit::command_buffer_begin_info();
        vkBeginCommandBuffer(frame.mainCommandBuffer, &beginInfo);

        // Now we need to transition the swapchain image to a layout that our render pass can use
        vkutil::transition_image(frame.mainCommandBuffer, swapchainImages[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        // Start the render pass
        VkClearValue clearColor = { .color = {this->clearColor.r, this->clearColor.g, this->clearColor.b, this->clearColor.a} };
        VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(swapchainImageViews[imageIndex], &clearColor, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkRenderingInfo renderInfo = vkinit::rendering_info(context.swapchainExtent, &colorAttachment, nullptr);

        vkCmdBeginRendering(frame.mainCommandBuffer, &renderInfo);

        // Now just for testing, we'll draw a triangle
        draw_geometry(frame.mainCommandBuffer, camera);

        for (auto &drawable : drawables) {
            drawable(frame.mainCommandBuffer, _drawImage.imageView);
        }

        vkCmdEndRendering(frame.mainCommandBuffer);

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
        init_default_data();

        init_mesh_pipeline();

        ShaderManager shaderManager = ShaderManager(); // Create the shader manager

        this->initialized = true;
    }



    void VulkanBackend::init_vulkan() {
        vkb::InstanceBuilder builder;

        auto extensions = Window::get_extensions();

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

        vkb::PhysicalDeviceSelector selector {vkb_inst};
        vkb::PhysicalDevice physical_device = selector.set_surface(context.surface)
            .set_required_features_13(vulkan13Features)
            .set_required_features_12(vulkan12Features)
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
        create_swapchain(context.window->width, context.window->height);

        VkExtent3D imageExtent = { .width = context.swapchainExtent.width, .height = context.swapchainExtent.height, .depth = 1 };
        this->_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT; // So we can use HDR, the swapchain differs from this, but that's fine it'll just be clamped
        this->_drawImage.imageExtent = imageExtent;

        VkImageUsageFlags usage {};
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

        VkImageCreateInfo imageInfo = vkinit::image_create_info(this->_drawImage.imageFormat, usage, imageExtent);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vmaCreateImage(context.allocator, &imageInfo, &allocInfo, &this->_drawImage.image, &this->_drawImage.allocation, nullptr);

        VkImageViewCreateInfo viewInfo = vkinit::image_view_create_info(this->_drawImage.imageFormat, this->_drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

        auto r = vkCreateImageView(context.device, &viewInfo, nullptr, &this->_drawImage.imageView);
        if (r != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view!");
        }

        context.mainDeletionQueue.push_function([=]() {
            vkDestroyImageView(context.device, this->_drawImage.imageView, nullptr);
            vmaDestroyImage(context.allocator, this->_drawImage.image, this->_drawImage.allocation);
        });
    }

    void VulkanBackend::create_swapchain(unsigned int width, unsigned int height, bool hdr) {
        vkb::SwapchainBuilder swapchainBuilder {context.physicalDevice, context.device, context.surface};

        this->context.swapchainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;

        if (hdr) {
            this->context.swapchainImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            this->context.colorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT;
        }

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            //.use_default_format_selection()
            .set_desired_format(VkSurfaceFormatKHR{ .format = this->context.swapchainImageFormat, .colorSpace = this->context.colorSpace })
            //use vsync present mode
            .set_desired_present_mode(this->context.presentMode)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
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

        this->drawables.push_back([&](VkCommandBuffer cb,VkImageView view) {
            ImGui::ShowDemoWindow();

            imguiBackend.draw(cb, view);
        });
    }

    void VulkanBackend::init_mesh_pipeline() {
/*
        VkShaderModule triangleFragShader;
        if (!vkutil::load_shader_module(context.device, "Shaders/colored_triangle.frag.spv", &triangleFragShader)) {
std::cout << "Failed to load triangle fragment shader!" << std::endl;
            throw std::runtime_error("Failed to load triangle fragment shader!");
        }

        VkShaderModule triangleVertShader;
        if (!vkutil::load_shader_module(context.device, "Shaders/colored_triangle.vert.spv", &triangleVertShader)) {
            throw std::runtime_error("Failed to load triangle vertex shader!");
        }

        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(Vulkan::GPUDrawPushConstants);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::pipeline_layout_create_info();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        vkutil::VK_CHECK(vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, nullptr, &_meshPipelineLayout));

        Vulkan::PipelineBuilder pipelineBuilder;
        pipelineBuilder.pipelineLayout = _meshPipelineLayout;
        //connecting the vertex and pixel shaders to the pipeline
        pipelineBuilder.set_shaders(triangleVertShader, triangleFragShader);
        //it will draw triangles
        pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        //filled triangles
        pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
        //no backface culling
        pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        //no multisampling
        pipelineBuilder.set_multisampling_none();
        //no blending
        pipelineBuilder.disable_blend();
        //no depth testing
        pipelineBuilder.set_depth_test(true);

        // Connect the pipeline to the rendering info
        pipelineBuilder.set_color_attachment_format(context.swapchainImageFormat);
        pipelineBuilder.set_depth_attachment_format(VK_FORMAT_D32_SFLOAT);

        _meshPipeline = pipelineBuilder.build(context.device);

        vkDestroyShaderModule(context.device, triangleFragShader, nullptr);
        vkDestroyShaderModule(context.device, triangleVertShader, nullptr);

        context.mainDeletionQueue.push_function([=]() {
            vkDestroyPipeline(context.device, _meshPipeline, nullptr);
            vkDestroyPipelineLayout(context.device, _meshPipelineLayout, nullptr);
        });
*/
    }

    void VulkanBackend::init_default_data() {
/*
        std::array<Vulkan::Vertex,4> rect_vertices;
        rect_vertices[0].position = {0.5,-0.5, 0};
        rect_vertices[1].position = {0.5,0.5, 0};
        rect_vertices[2].position = {-0.5,-0.5, 0};
        rect_vertices[3].position = {-0.5,0.5, 0};

        rect_vertices[0].color = {0,0, 0,1};
        rect_vertices[1].color = { 0.5,0.5,0.5 ,1};
        rect_vertices[2].color = { 1,0, 0,1 };
        rect_vertices[3].color = { 0,1, 0,1 };

        std::array<uint32_t,6> rect_indices = {0,1,2,2,1,3};

        rectangle = upload_mesh(rect_indices, rect_vertices);

        context.mainDeletionQueue.push_function([=]() {
            vmaDestroyBuffer(context.allocator, rectangle.vertexBuffer.buffer, rectangle.vertexBuffer.allocation);
            vmaDestroyBuffer(context.allocator, rectangle.indexBuffer.buffer, rectangle.indexBuffer.allocation);
        });
*/
    }


    void VulkanBackend::draw_geometry(VkCommandBuffer cmd, Camera &camera) {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipeline);

        //set dynamic viewport and scissor
        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = (float) context.swapchainExtent.width;
        viewport.height = (float) context.swapchainExtent.height;

        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = context.swapchainExtent.width;
        scissor.extent.height = context.swapchainExtent.height;

        vkCmdSetScissor(cmd, 0, 1, &scissor);

        Vulkan::GPUDrawPushConstants pushConstants;
        pushConstants.model = glm::mat4(1.0f);
        pushConstants.view = camera.getViewMatrix();
        pushConstants.projection = camera.getProjectionMatrix();
        pushConstants.meshBufferAddress = rectangle.vertexBufferDeviceAddress;

        vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Vulkan::GPUDrawPushConstants), &pushConstants);
        vkCmdBindIndexBuffer(cmd, rectangle.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        //launch a draw command to draw 6 vertices
        vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
    }
} // Graphics