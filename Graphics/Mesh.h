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

namespace Graphics::Mesh {

    class Mesh {
    public:
        Mesh(const std::string &pipelineName);
        ~Mesh();

        void setVertices(float* vertices, unsigned int size);
        void setIndices(unsigned int* indices, unsigned int size);
        void setPipeline(std::string pipelineName);
        void createBuffers();

        int getVerticesSize() const;
        int getIndicesSize() const;

        void draw(VkCommandBuffer commandBuffer, Other::Transform transform, Camera camera, VkDescriptorSet descriptorSet) const;

    private:
        bool ready = false;

        int verticesSize = 0;
        int indicesSize = 0;

        float* vertices = nullptr;
        unsigned int* indices = nullptr;

        Vulkan::GPUMeshBuffers gpuMeshBuffers;

        std::string pipelineName;
        VkPipeline pipeline = VK_NULL_HANDLE; // For faster lookup
    };

}

#endif //MESH_H
