//
// Created by marti on 23/09/2024.
//

#include "DescriptorLayoutBuilder.h"

#include <stdexcept>
#include <string>

#include "../vkutil.h"

void DescriptorLayoutBuilder::add_binding(uint32_t binding, VkDescriptorType type) {
    VkDescriptorSetLayoutBinding  newbind{};
    newbind.binding = binding;
    newbind.descriptorType = type;
    newbind.descriptorCount = 1;
    newbind.stageFlags = VK_SHADER_STAGE_ALL;
    newbind.pImmutableSamplers = nullptr;

    bindings.push_back(newbind);
}

void DescriptorLayoutBuilder::clear() {
    bindings.clear();
}



VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device, VkShaderStageFlags shaderStages, void *pNext,
                                                     VkDescriptorSetLayoutCreateFlags flags) {
    for (auto& b : bindings) {
        b.stageFlags |= shaderStages;
    }

    VkDescriptorSetLayoutCreateInfo info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    info.pNext = pNext;

    info.pBindings = bindings.data();
    info.bindingCount = (uint32_t)bindings.size();
    info.flags = flags;

    VkDescriptorSetLayout set;
    vkutil::VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

    return set;

}

void DescriptorLayoutBuilder::add_binding(uint32_t binding, VkDescriptorType type, uint32_t count,
                                          VkShaderStageFlags stageFlags) {
    VkDescriptorSetLayoutBinding  newbind{};
    newbind.binding = binding;
    newbind.descriptorType = type;
    newbind.descriptorCount = count;
    newbind.stageFlags = stageFlags;
    newbind.pImmutableSamplers = nullptr;

    bindings.push_back(newbind);
}

void DescriptorLayoutBuilder::add_binding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags) {
    VkDescriptorSetLayoutBinding  newbind{};
    newbind.binding = binding;
    newbind.descriptorType = type;
    newbind.descriptorCount = 1;
    newbind.stageFlags = stageFlags;
    newbind.pImmutableSamplers = nullptr;

    bindings.push_back(newbind);
}
