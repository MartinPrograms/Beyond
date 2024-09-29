//
// Created by marti on 29/09/2024.
//

#include "MeshUtils.h"

#include "VulkanContext.h"
#include "../vkinit.h"

namespace Graphics {
namespace Vulkan {
    Graphics::Vulkan::GPUMeshBuffers MeshUtils::upload_mesh(std::span<uint32_t> indices,
            std::span<Graphics::Vulkan::Vertex> vertices) {
        auto contextPtr = Graphics::Vulkan::VulkanContext::getInstance();
        auto context = *contextPtr;

        const size_t vertexBufferSize = vertices.size() * sizeof(Vulkan::Vertex);
        const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

        Vulkan::GPUMeshBuffers newSurface;

        //create vertex buffer
        newSurface.vertexBuffer = Vulkan::AllocatedBuffer::create_buffer(context.allocator, vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

        //find the adress of the vertex buffer
        VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = newSurface.vertexBuffer.buffer };
        newSurface.vertexBufferDeviceAddress = vkGetBufferDeviceAddress(context.device, &deviceAdressInfo);

        //create index buffer
        newSurface.indexBuffer = Vulkan::AllocatedBuffer::create_buffer(context.allocator,indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

        Vulkan::AllocatedBuffer stagingBuffer = Vulkan::AllocatedBuffer::create_buffer(context.allocator, vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU);

        void* data;
        VkResult result = vmaMapMemory(context.allocator, stagingBuffer.allocation, &data);
        if (result == VK_SUCCESS) {
            // You can now access the mapped memory through `data`
            memcpy(data, vertices.data(), vertexBufferSize);
            memcpy((char*) data + vertexBufferSize, indices.data(), indexBufferSize);

            std::cout << "Starting immediate submit!" << std::endl;
            immediate_submit([&](VkCommandBuffer cmd) {
                VkBufferCopy vertexCopy{ 0 };
                vertexCopy.dstOffset = 0;
                vertexCopy.srcOffset = 0;
                vertexCopy.size = vertexBufferSize;

                vkCmdCopyBuffer(cmd, stagingBuffer.buffer, newSurface.vertexBuffer.buffer, 1, &vertexCopy);

                VkBufferCopy indexCopy{ 0 };
                indexCopy.dstOffset = 0;
                indexCopy.srcOffset = vertexBufferSize;
                indexCopy.size = indexBufferSize;

                vkCmdCopyBuffer(cmd, stagingBuffer.buffer, newSurface.indexBuffer.buffer, 1, &indexCopy);
                    });
            vmaUnmapMemory(context.allocator, stagingBuffer.allocation);

            stagingBuffer.destroy(context.allocator);
        } else {
            // Handle error
            std::cout << "Failed to map memory!" << std::endl;
        }

        // Don't forget to unmap the memory after you're done
        std::cout << "Unmapped memory!" << std::endl;
        return newSurface;

    }

    void MeshUtils::immediate_submit(std::function<void(VkCommandBuffer cmd)> &&function) {
        auto contextPtr = Graphics::Vulkan::VulkanContext::getInstance();
        auto context = *contextPtr;

        vkutil::VK_CHECK(vkResetFences(context.device, 1, &context.immediateFence));
        vkutil::VK_CHECK(vkResetCommandPool(context.device, context.immediateCommandPool, 0));

        VkCommandBuffer cmd = context.immediateCommandBuffer;

        VkCommandBufferBeginInfo beginInfo = vkinit::command_buffer_begin_info();

        vkutil::VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

        function(cmd);

        vkutil::VK_CHECK(vkEndCommandBuffer(cmd));

        VkCommandBufferSubmitInfo cmdInfo = vkinit::command_buffer_submit_info(cmd);
        VkSubmitInfo2 submitInfo = vkinit::submit_info(&cmdInfo, nullptr, nullptr);

        vkutil::VK_CHECK(vkQueueSubmit2(context.graphicsQueue, 1, &submitInfo, context.immediateFence));

        vkutil::VK_CHECK(vkWaitForFences(context.device, 1, &context.immediateFence, VK_TRUE, 1000000000)); // a second

    }
} // Vulkan
} // Graphics