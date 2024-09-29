//
// Created by marti on 29/09/2024.
//

#include "Mesh.h"

#include "ShaderManager.h"
#include "Vulkan/VulkanContext.h"

namespace Graphics {
namespace Mesh {
    Mesh::Mesh(const std::string &pipelineName) {
        this->pipelineName = pipelineName;
        this->pipeline = ShaderManager::getInstance()->getPipeline(pipelineName);

        if (pipeline == VK_NULL_HANDLE) {
            throw std::runtime_error("Pipeline not found");
        }
    }

    Mesh::~Mesh() {

    }

    void Mesh::setVertices(float *vertices, unsigned int size) {
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

    void Mesh::draw(VkCommandBuffer commandBuffer, Other::Transform transform, Camera camera,
        VkDescriptorSet descriptorSet) const {
        // The magic happens here
    }

    void Mesh::createBuffers() {
        // some magic here too

    }
} // Mesh
} // Graphics