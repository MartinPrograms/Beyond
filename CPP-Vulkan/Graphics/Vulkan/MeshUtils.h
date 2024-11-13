//
// Created by marti on 29/09/2024.
//

#ifndef MESHUTILS_H
#define MESHUTILS_H
#include <span>

#include "Structs.h"
#include "VulkanContext.h"
#include "../Mesh.h"

namespace Graphics {
namespace Vulkan {

class MeshUtils {
public:

    explicit MeshUtils(VulkanContext& context);
    static Graphics::Vulkan::GPUMeshBuffers upload_mesh(std::span<uint32_t> indices, std::span<Graphics::Vulkan::Vertex> vertices, VulkanContext& context);
    static void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function, VulkanContext& context);

    Mesh::Mesh loadMesh(std::string path, int index, std::string pipelineName);
    Mesh::Mesh loadMesh(std::span<uint32_t> indices, std::span<Graphics::Vulkan::Vertex> vertices, std::string pipelineName);

private:
    VulkanContext& context;
};
} // Vulkan
} // Graphics

#endif //MESHUTILS_H
