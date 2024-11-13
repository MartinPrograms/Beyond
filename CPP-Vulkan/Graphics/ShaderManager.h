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

    // Singleton class, meant for managing pipelines, so that we don't have to create a new pipeline for every mesh (which is very inefficient... Especially if we have a lot of meshes)
class ShaderManager {
public:

    static ShaderManager* getInstance() {
        return instance;
    }

    ShaderManager() {
        instance = this;
    }

    VkPipeline getPipeline(const std::string& name) {
        if (!pipelines.contains(name)) {
            return VK_NULL_HANDLE;
        }

        return pipelines[name];
    }

    VkPipelineLayout getPipelineLayout(const std::string &string) {
        return pipelineLayouts[string];
    }

    void destroyPipeline(const char * str) {
        if (pipelines.contains(str)) {
            vkDestroyPipeline(Vulkan::VulkanContext::getDevice(), pipelines[str], nullptr);
            pipelines.erase(str);
        }
    }

    Vulkan::DeletionQueue deletionQueue;
    static ShaderManager *instance;

    // The gpu push constants are in the pipeline layout, so we don't need to pass them here
    VkPipeline createPipeline(const std::string& name, const char* frag, const char* vert, VkDevice device, VkPipelineLayout pipelineLayout, VkFormat colorAttachmentFormat, VkFormat depthAttachmentFormat, VkPolygonMode polygon_mode, VkCullModeFlags cull_mode) {

        std::cout << 1 << std::endl;
        if (pipelines.contains(name)) {
            std::cout << "Pipeline with name " << name << " already exists!" << std::endl;
            return pipelines[name];
        }

        std::cout << 2 << std::endl;

        VkShaderModule triangleFragShader;
        if (!vkutil::load_shader_module(device, frag, &triangleFragShader)) {
        std::cout << "Failed to load triangle fragment shader!" << std::endl;
            throw std::runtime_error("Failed to load triangle fragment shader!");
        }

        std::cout << 3 << std::endl;

        VkShaderModule triangleVertShader;
        if (!vkutil::load_shader_module(device, vert, &triangleVertShader)) {
            throw std::runtime_error("Failed to load triangle vertex shader!");
        }

        std::cout << "Creating pipeline" << std::endl;

        Vulkan::PipelineBuilder pipelineBuilder;
        pipelineBuilder.pipelineLayout = pipelineLayout;
        pipelineLayouts.insert({name, pipelineLayout});
        //connecting the vertex and pixel shaders to the pipeline
        pipelineBuilder.set_shaders(triangleVertShader, triangleFragShader);
        //it will draw triangles
        pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        //filled triangles
        pipelineBuilder.set_polygon_mode(polygon_mode);
        //no backface culling
        pipelineBuilder.set_cull_mode(cull_mode, VK_FRONT_FACE_CLOCKWISE);
        //no multisampling
        pipelineBuilder.set_multisampling_none();
        //no blending
        pipelineBuilder.disable_blend();
        //no depth testing
        pipelineBuilder.set_depth_test(true);

        // Connect the pipeline to the rendering info
        pipelineBuilder.set_color_attachment_format(colorAttachmentFormat);
        pipelineBuilder.set_depth_attachment_format(depthAttachmentFormat);

        std::cout << "Building pipeline" << std::endl;

        VkPipeline pipeline = pipelineBuilder.build(device);

        std::cout << "Pipeline built" << std::endl;

        vkDestroyShaderModule(device, triangleFragShader, nullptr);
        vkDestroyShaderModule(device, triangleVertShader, nullptr);

        deletionQueue.push_function([device, pipeline]() {
            vkDestroyPipeline(device, pipeline, nullptr);
        });

        pipelines.insert({name, pipeline});
        return pipelines[name] = pipeline;
    }

private:
    std::unordered_map<std::string, VkPipeline> pipelines= std::unordered_map<std::string, VkPipeline>();
    std::unordered_map<std::string, VkPipelineLayout> pipelineLayouts = std::unordered_map<std::string, VkPipelineLayout>();
};

} // Graphics

#endif //SHADERMANAGER_H
