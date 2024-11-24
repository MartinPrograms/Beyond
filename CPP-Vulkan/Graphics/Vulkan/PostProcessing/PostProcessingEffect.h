//
// Created by marti on 23/11/2024.
//

#ifndef POSTPROCESSINGEFFECT_H
#define POSTPROCESSINGEFFECT_H

#include <vector>
#include <vulkan/vulkan_core.h>
#include <vma/vk_mem_alloc.h>
#include "../VulkanContext.h"

namespace Graphics::Mesh {
    class Mesh;
}

namespace Graphics::Vulkan::PostProcessing {

    enum PostProcessingEffectType {
        BLUR, // 0
        SSAO, // 1
    };

    class PostProcessingEffect {
    public:
        virtual ~PostProcessingEffect() = default;
        PostProcessingEffect();

        // Initialize resources required for the effect.
        virtual void initialize(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, VkExtent2D swapchainExtent, char* vertex_path, char* fragment_path) = 0;

        // Record commands for this effect into the given command buffer.
        virtual void recordCommands(VkCommandBuffer commandBuffer, VkExtent2D swapchainExtent, VkImage inputImage, VkImage outputImage) = 0;

        // Cleanup resources associated with the effect.
        virtual void cleanup(VkDevice device) = 0;

    protected:
        // Utility function for creating Vulkan resources (to be shared by derived classes).
        VkImage createImage(VmaAllocator allocator, VkExtent2D extent, VkFormat format, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocation& allocation);

        VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

        VkSampler createSampler(VkDevice device, float maxLod = 1.0f);

        VkFramebuffer createFramebuffer(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent);

        // A utility function for creating shaders, useful for post-processing effects.
        VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

        static Mesh::Mesh* renderMesh;
    };

}

#endif //POSTPROCESSINGEFFECT_H
