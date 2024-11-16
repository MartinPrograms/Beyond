//
// Created by marti on 29/09/2024.
//

#ifndef MESH_H
#define MESH_H
#include <string>

#include "Camera.h"
#include "Other/Transform.h"
#include "Vulkan/DescriptorLayoutBuilder.h"
#include "Vulkan/Structs.h"
#include "Vulkan/VulkanContext.h"

namespace Graphics::Mesh {

    class Mesh {
    public:
        Mesh(const std::string &pipelineName);
        ~Mesh();

        void setVertices(Graphics::Vulkan::Vertex* vertices, unsigned int size);
        void setIndices(unsigned int *indices, unsigned int size);

        void setPipeline(std::string pipelineName);
        void createBuffers(Vulkan::VulkanContext &context);

        int getVerticesSize() const;
        int getIndicesSize() const;

        void draw(VkCommandBuffer commandBuffer, Camera* camera, VkImageView image,VkDescriptorSet descriptor_set);

        void destroy(const Vulkan::VulkanContext &context) const;
        Other::Transform transform;

    private:
        bool ready = false;

        int verticesSize = 0;
        int indicesSize = 0;

        Graphics::Vulkan::Vertex* vertices = nullptr;
        unsigned int* indices = nullptr;

        Vulkan::GPUMeshBuffers gpuMeshBuffers;

        std::string pipelineName;
        VkPipeline pipeline = VK_NULL_HANDLE; // For faster lookup

    };

}

#endif //MESH_H
