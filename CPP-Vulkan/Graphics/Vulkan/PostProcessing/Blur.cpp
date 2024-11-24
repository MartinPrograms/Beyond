//
// Created by marti on 23/11/2024.
//

#include "Blur.h"

#include "../PipelineBuilder.h"
#include "../../ShaderManager.h"
#include "../../vkinit.h"

void Blur::initialize(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue,
                      VkExtent2D swapchainExtent,char* vertex_path, char* fragment_path) {
    // Calculate the size of the downsampled image
    VkExtent2D downsampledExtent = { swapchainExtent.width / blurStrength, swapchainExtent.height / blurStrength };

    this->blurImage = createImage(Graphics::Vulkan::VulkanContext::getInstance()->allocator, downsampledExtent, Graphics::Vulkan::VulkanContext::getInstance()->swapchainImageFormat,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, this->blurImageAllocation);

    this->blurFramebuffer = createFramebuffer(device, blurRenderPass, { blurImageView }, downsampledExtent);
    this->blurImageAllocation = blurImageAllocation;
    this->blurImageView = createImageView(device, blurImage, Graphics::Vulkan::VulkanContext::getInstance()->swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);


    // Create the pipeline layout
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstant);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::pipeline_layout_create_info();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    createBlurPipeline(device, vertex_path, fragment_path);
}

void Blur::recordCommands(VkCommandBuffer commandBuffer, VkExtent2D swapchainExtent, VkImage inputImage, VkImage outputImage) {

}

void Blur::cleanup(VkDevice device) {
    vkDestroyImageView(device, blurImageView, nullptr);
    vmaDestroyImage(Graphics::Vulkan::VulkanContext::getInstance()->allocator, blurImage, blurImageAllocation);
    vkDestroyFramebuffer(device, blurFramebuffer, nullptr);
    vkDestroyPipeline(device, blurPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, blurRenderPass, nullptr);
}

void Blur::createBlurPipeline(VkDevice device,char* vertex_path, char* fragment_path) {
    auto manager = Graphics::ShaderManager::getInstance();
    manager->createPipeline("blur", fragment_path, vertex_path, device, pipelineLayout, Graphics::Vulkan::VulkanContext::getInstance()->swapchainImageFormat, VK_FORMAT_D32_SFLOAT, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT);
}
