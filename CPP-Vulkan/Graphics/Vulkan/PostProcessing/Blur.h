//
// Created by marti on 23/11/2024.
//

#ifndef BLUR_H
#define BLUR_H
#include "PostProcessingEffect.h"


class Blur: public Graphics::Vulkan::PostProcessing::PostProcessingEffect {
public:

    struct PushConstant {
        float blurStrength;
        float width;
        float height;
        float padding;
    };

    int blurStrength = 4; // SwapchainExtent/blurStrength = blur size, and it uses downscaling to make it faster.
    void initialize(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue, VkExtent2D swapchainExtent,char* vertex_path, char* fragment_path) override;
    void recordCommands(VkCommandBuffer commandBuffer, VkExtent2D swapchainExtent, VkImage inputImage, VkImage outputImage) override;
    void cleanup(VkDevice device) override;

private:
    VkRenderPass blurRenderPass;
    VkPipeline blurPipeline;
    VkPipelineLayout pipelineLayout;
    VkFramebuffer blurFramebuffer;

    VmaAllocation blurImageAllocation;
    VkImage blurImage;
    VkImageView blurImageView;

    void createBlurPipeline(VkDevice device, char* vertex_path, char* fragment_path);
};



#endif //BLUR_H
