//
// Created by marti on 24/09/2024.
//

#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "Structs.h"
#include "../../Window.h"
#include "../../FidelityFX/include/ffx_api.h"

namespace Graphics {
namespace Vulkan {


class VulkanContext {
// Because C++ is a pain in the yk what, i have to do this to avoid a circular dependency (howwwwwwwwwwwwwwww is this even a thing)
public:
public:
    static VulkanContext* getInstance() {
        if (!VulkanContextInstance) {
            VulkanContextInstance = new VulkanContext();
        }
        return VulkanContextInstance;
    }

    static VkDevice getDevice() {
        if (!VulkanContextInstance) {
            throw std::runtime_error("VulkanContext has not been initialized!");
        }
        return VulkanContextInstance->device;
    }

    static VkPhysicalDevice getPhysicalDevice() {
        if (!VulkanContextInstance) {
            throw std::runtime_error("VulkanContext has not been initialized!");
        }
        return VulkanContextInstance->physicalDevice;
    }

    VulkanContext() {
        if (VulkanContextInstance != nullptr) {
            std::cout << "istg this is magic" << std::endl;
            // just return the instance
            return;
        }
        VulkanContextInstance = this;
        this->window = nullptr;
        this->swapchainExtent = {0, 0};
        this->swapchain = VK_NULL_HANDLE;
        this->allocator = VK_NULL_HANDLE;
        this->immediateFence = VK_NULL_HANDLE;
    }

    ~VulkanContext() {
        VulkanContextInstance = nullptr;
    }

    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
    VkSurfaceKHR surface{};
    VkDebugUtilsMessengerEXT debugMessenger{};
    Window* window{};
    VkQueue graphicsQueue{};
    unsigned int graphicsQueueFamily{};

    VkFormat swapchainImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkExtent2D swapchainExtent{};
    VkSwapchainKHR swapchain{};

    DeletionQueue mainDeletionQueue;

    VmaAllocator allocator{};

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // No vsync

    // Immediate mode
    VkFence immediateFence{};
    VkCommandPool immediateCommandPool{};
    VkCommandBuffer immediateCommandBuffer{};

    static VulkanContext* VulkanContextInstance;

    // FFX
    ffxContext ffxContext{};


// Swapchain images are not owned by us, so we don't need to have them here
};
} // Vulkan
} // Graphics

#endif //VULKANCONTEXT_H
