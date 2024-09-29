//
// Created by marti on 23/09/2024.
//

#ifndef STRUCTS_H
#define STRUCTS_H
#include <deque>
#include <functional>
#include "../vkutil.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"


namespace Graphics::Vulkan {

    struct AllocatedImage {
        VkImage image;
        VkImageView imageView;
        VmaAllocation allocation;
        VkExtent3D imageExtent;
        VkFormat imageFormat;
    };

    struct AllocatedBuffer {
        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo info;

        static AllocatedBuffer create_buffer(VmaAllocator _allocator, size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
            VkBufferCreateInfo bufferInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufferInfo.pNext = nullptr;
            bufferInfo.size = allocSize;

            bufferInfo.usage = usage;

            VmaAllocationCreateInfo vmaallocInfo = {};
            vmaallocInfo.usage = memoryUsage;
            vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
            AllocatedBuffer newBuffer;

            // allocate the buffer
            vkutil::VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo, &newBuffer.buffer, &newBuffer.allocation,
                &newBuffer.info));

            return newBuffer;
        }

        void destroy(VmaAllocator allocator) const {
            vmaDestroyBuffer(allocator, buffer, allocation);
        }
    };
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void push_function(std::function<void()>&& function) {
            deletors.push_back(function);
        }

        void flush() {
            // reverse iterate the deletion queue to execute all the functions
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
                (*it)(); //call functors
            }

            deletors.clear();
        }
    };

    struct FrameData {
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence renderFence;

        VkCommandPool commandPool;
        VkCommandBuffer mainCommandBuffer;

        DeletionQueue _deletionQueue;
    };

    struct Vertex {
        glm::vec3 position;
        float uv_x;
        glm::vec3 normal;
        float uv_y;
        glm::vec4 color;
    };

    struct GPUMeshBuffers {
        AllocatedBuffer indexBuffer;
        AllocatedBuffer vertexBuffer;
        VkDeviceAddress vertexBufferDeviceAddress;
    };

    struct GPUDrawPushConstants {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        VkDeviceAddress meshBufferAddress;
    };

}

#endif //STRUCTS_H
