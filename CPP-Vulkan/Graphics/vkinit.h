//
// Created by marti on 23/09/2024.
//

#ifndef VKINIT_H
#define VKINIT_H
#include <vulkan/vulkan_core.h>


class vkinit {
public:
    static VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
    static VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    static VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);
    static VkSemaphoreCreateInfo semaphore_create_info();
    static VkCommandBufferBeginInfo command_buffer_begin_info();
    static VkImageSubresourceRange image_subresource_range(VkImageAspectFlags aspectMask, uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount);

    static VkSemaphoreSubmitInfo semaphore_submit_info(VkSemaphore semaphore, VkPipelineStageFlags2 stageFlags);
    static VkCommandBufferSubmitInfo command_buffer_submit_info(VkCommandBuffer cmd);
    static VkSubmitInfo2 submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
        VkSemaphoreSubmitInfo* waitSemaphoreInfo);
    static VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
    static VkImageViewCreateInfo image_view_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
    static VkRenderingAttachmentInfo attachment_info(VkImageView view, VkClearValue* clear ,VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

    static VkRenderingInfo rendering_info(VkExtent2D renderExtent, VkRenderingAttachmentInfo* colorAttachment,
    VkRenderingAttachmentInfo* depthAttachment);

    static VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits vk_shader_stage_flag_bits, VkShaderModule vk_shader_module);

    static VkPipelineLayoutCreateInfo pipeline_layout_create_info();

    static VkRect2D scissor(VkExtent2D vk_extent_2d);
    static VkViewport viewport(VkExtent2D vk_extent_2d);
};


#endif //VKINIT_H
