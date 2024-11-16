//
// Created by marti on 29/09/2024.
//

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H
#include <iostream>
#include <string>
#include <unordered_map>

#include "vkutil.h"
#include "Vulkan/PipelineBuilder.h"
#include "Vulkan/Structs.h"
#include "Vulkan/VulkanContext.h"

namespace Graphics {

class ShaderManager {
public:
    // Singleton access method
    static ShaderManager* getInstance() {
        static ShaderManager instance; // Guaranteed to be lazy-initialized and thread-safe (C++11 and later)
        return &instance;
    }

    ShaderManager(const ShaderManager&) = delete; // Delete copy constructor
    ShaderManager& operator=(const ShaderManager&) = delete; // Delete copy assignment

    VkPipeline getPipeline(const std::string& name) {
        if (pipelines.find(name) == pipelines.end()) {
            return VK_NULL_HANDLE;
        }
        return pipelines[name];
    }

    VkPipelineLayout getPipelineLayout(const std::string& name) {
        if (pipelineLayouts.find(name) == pipelineLayouts.end()) {
            return VK_NULL_HANDLE;
        }
        return pipelineLayouts[name];
    }

    void destroyPipeline(const std::string& name) {
        if (pipelines.find(name) != pipelines.end()) {
            vkDestroyPipeline(Vulkan::VulkanContext::getDevice(), pipelines[name], nullptr);
            pipelines.erase(name);
        }
    }

    VkPipeline createPipeline(const std::string& name, const char* frag, const char* vert, VkDevice device,
                              VkPipelineLayout pipelineLayout, VkFormat colorAttachmentFormat, VkFormat depthAttachmentFormat,
                              VkPolygonMode polygon_mode, VkCullModeFlags cull_mode) {

        if (pipelines.find(name) != pipelines.end()) {
            std::cout << "Pipeline with name " << name << " already exists!" << std::endl;
            return pipelines[name];
        }

        VkShaderModule fragShaderModule;
        if (!vkutil::load_shader_module(device, frag, &fragShaderModule)) {
            std::cout << "Failed to load fragment shader!" << std::endl;
            throw std::runtime_error("Failed to load fragment shader!");
        }

        VkShaderModule vertShaderModule;
        if (!vkutil::load_shader_module(device, vert, &vertShaderModule)) {
            std::cout << "Failed to load vertex shader!" << std::endl;
            throw std::runtime_error("Failed to load vertex shader!");
        }

        Vulkan::PipelineBuilder pipelineBuilder;
        pipelineBuilder.pipelineLayout = pipelineLayout;
        pipelineLayouts[name] = pipelineLayout;

        pipelineBuilder.set_shaders(vertShaderModule, fragShaderModule);
        pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.set_polygon_mode(polygon_mode);
        pipelineBuilder.set_cull_mode(cull_mode, VK_FRONT_FACE_CLOCKWISE);
        pipelineBuilder.set_multisampling_none();
        pipelineBuilder.disable_blend();
        pipelineBuilder.set_depth_test(true);
        pipelineBuilder.set_color_attachment_format(colorAttachmentFormat);
        pipelineBuilder.set_depth_attachment_format(depthAttachmentFormat);

        VkPipeline pipeline = pipelineBuilder.build(device);

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);

        deletionQueue.push_function([device, pipeline]() {
            vkDestroyPipeline(device, pipeline, nullptr);
        });

        pipelines[name] = pipeline;
        return pipeline;
    }

    Vulkan::DeletionQueue deletionQueue;

private:
    ShaderManager() = default; // Private constructor for singleton pattern

    std::unordered_map<std::string, VkPipeline> pipelines;
    std::unordered_map<std::string, VkPipelineLayout> pipelineLayouts;
};

} // namespace Graphics

#endif // SHADERMANAGER_H