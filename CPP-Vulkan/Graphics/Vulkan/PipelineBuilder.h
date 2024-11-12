//
// Created by marti on 29/09/2024.
//

#ifndef PIPELINEBUILDER_H
#define PIPELINEBUILDER_H
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Graphics::Vulkan {

    class PipelineBuilder {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineLayout pipelineLayout;
        VkPipelineDepthStencilStateCreateInfo depthStencil;
        VkPipelineRenderingCreateInfo renderInfo;
        VkFormat colorAttachmentformat;

        PipelineBuilder(){clear();}

        void clear();
        void set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void set_input_topology(VkPrimitiveTopology topology);
        void set_polygon_mode(VkPolygonMode mode);
        void set_cull_mode(VkCullModeFlags mode, VkFrontFace face);
        void set_multisampling_none();
        void disable_blend();
        void set_depth_test(bool enable);
        void set_color_attachment_format(VkFormat format);
        void set_depth_attachment_format(VkFormat format);

        VkPipeline build(VkDevice device);
    };

}

#endif //PIPELINEBUILDER_H
