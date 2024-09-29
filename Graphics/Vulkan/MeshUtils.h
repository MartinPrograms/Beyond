//
// Created by marti on 29/09/2024.
//

#ifndef MESHUTILS_H
#define MESHUTILS_H
#include <span>

#include "Structs.h"

namespace Graphics {
namespace Vulkan {

class MeshUtils {
public:

    static Graphics::Vulkan::GPUMeshBuffers upload_mesh(std::span<uint32_t> indices, std::span<Graphics::Vulkan::Vertex> vertices);
    static void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
};

} // Vulkan
} // Graphics

#endif //MESHUTILS_H
