//
// Created by marti on 29/09/2024.
//

#include "Mesh.h"

#include "ShaderManager.h"
#include "Vulkan/MeshUtils.h"
#include "Vulkan/VulkanContext.h"
#include "Other/Transform.h"

namespace Graphics {
namespace Mesh {
    Mesh::Mesh(const std::string &pipelineName) {
        this->pipelineName = pipelineName;
        auto shaderManager = ShaderManager::getInstance();
        this->pipeline = shaderManager->getPipeline(pipelineName);

        if (pipeline == VK_NULL_HANDLE) {
            std::cout << "Pipeline not found!" << std::endl;
            throw std::runtime_error("Pipeline not found");
        }

        transform = Other::Transform();
    }

    Mesh::~Mesh() {

    }

    void Mesh::setVertices(Graphics::Vulkan::Vertex* vertices, unsigned int size) {
        this->vertices = vertices;
        this->verticesSize = size;

    }

    void Mesh::setIndices(unsigned int *indices, unsigned int size) {
        this->indices = indices;
        this->indicesSize = size;
    }

    void Mesh::setPipeline(std::string pipelineName) {
        this->pipelineName = pipelineName;
        this->pipeline = ShaderManager::getInstance()->getPipeline(pipelineName);

        if (pipeline == VK_NULL_HANDLE) {
            throw std::runtime_error("Pipeline not found");
        }
    }

    int Mesh::getVerticesSize() const {
        return verticesSize;
    }

    int Mesh::getIndicesSize() const {
        return indicesSize;
    }

    void Mesh::draw(VkCommandBuffer commandBuffer, Camera* camera,
        VkImageView image, VkDescriptorSet descriptor_set) {
        if (pipeline == VK_NULL_HANDLE) {
            std::cout << "Pipeline not found!!!" << std::endl;
            // Before we crash, we should check if the pipeline is in the shader manager
            auto shaderManager = ShaderManager::getInstance();
            this->pipeline = shaderManager->getPipeline(pipelineName);
            if (pipeline == VK_NULL_HANDLE)
                throw std::runtime_error("Pipeline not found");
            std::cout << "Restored pipeline!" << std::endl;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        Vulkan::GPUDrawPushConstants pushConstants;
        pushConstants.model = transform.getModelMatrix();
        pushConstants.view = camera->getViewMatrix();
        pushConstants.projection = camera->getProjectionMatrix();
        pushConstants.meshBufferAddress = gpuMeshBuffers.vertexBufferDeviceAddress; // Weird how Vulkan uses this, unlike OpenGL where its VBO.Bind()

        vkCmdPushConstants(commandBuffer, ShaderManager::getInstance()->getPipelineLayout(pipelineName), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Vulkan::GPUDrawPushConstants), &pushConstants);
        vkCmdBindIndexBuffer(commandBuffer, gpuMeshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        // Now we draw the mesh
        //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ShaderManager::getInstance()->getPipelineLayout(pipelineName), 0, 1, &descriptor_set, 0, nullptr);
        vkCmdDrawIndexed(commandBuffer, indicesSize, 1, 0, 0, 0);

        // We do not end the render pass here, as we want to draw multiple meshes in the same render pass, otherwise we would have to create a new render pass for every mesh (which is very inefficient)
    }

    void Mesh::destroy(const Vulkan::VulkanContext &context) const {
        vmaDestroyBuffer(context.allocator, gpuMeshBuffers.vertexBuffer.buffer, gpuMeshBuffers.vertexBuffer.allocation);
        vmaDestroyBuffer(context.allocator, gpuMeshBuffers.indexBuffer.buffer, gpuMeshBuffers.indexBuffer.allocation);
        std::cout << "Destroyed mesh!" << std::endl;
    }

    void Mesh::createBuffers(Vulkan::VulkanContext &context) {
        auto buffer = Vulkan::MeshUtils::upload_mesh(std::span<uint32_t>(indices, indicesSize), std::span<Graphics::Vulkan::Vertex>(vertices, verticesSize), context);
        gpuMeshBuffers = buffer;
    }
} // Mesh
} // Graphics