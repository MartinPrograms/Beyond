//
// Created by marti on 24/09/2024.
//

#ifndef IMGUIBACKEND_H
#define IMGUIBACKEND_H
#include <vulkan/vulkan_core.h>

#include "../vkutil.h"
#include <imgui.h>

#include "VulkanContext.h"


namespace Graphics::Vulkan {

    class ImGuiBackend {
    public:
        explicit ImGuiBackend(VulkanContext* backend);
        ~ImGuiBackend();

        void init();
        void update(float dt);
        void draw(VkCommandBuffer cb,VkImageView view);


    private:
        VulkanContext* backend; // for the allocator, deletion queue, etc
    };

}

#endif //IMGUIBACKEND_H
