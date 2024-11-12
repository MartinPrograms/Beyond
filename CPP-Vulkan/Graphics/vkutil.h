//
// Created by marti on 23/09/2024.
//

#ifndef VKUTIL_H
#define VKUTIL_H

#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>


class vkutil {
public:
    static void VK_CHECK(VkResult vk_result) {
        if (vk_result != VK_SUCCESS) {
            throw std::runtime_error("Vulkan error: " + std::to_string(vk_result));
        }
    }
    static void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
    static void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);
    static bool load_shader_module(VkDevice device, const char* filename, VkShaderModule* out_module);

    static VkPipelineLayoutCreateInfo pipeline_layout_create_info(VkDescriptorSetLayout* vk_descriptor_set_layout, int i);

    static void check_vk_result(VkResult vk_result);

    /*
         Vulkan::GPUMeshBuffers VulkanBackend::upload_mesh(std::span<uint32_t> indices, std::span<Vulkan::Vertex> vertices) {
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
     */

};



#endif //VKUTIL_H
