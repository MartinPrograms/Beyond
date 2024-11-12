//
// Created by marti on 23/09/2024.
//

#ifndef DESCRIPTORLAYOUTBUILDER_H
#define DESCRIPTORLAYOUTBUILDER_H
#include <vector>
#include <vulkan/vulkan_core.h>


class DescriptorLayoutBuilder {
public:
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  void add_binding(uint32_t binding, VkDescriptorType type);
  void clear();
  VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);

  void add_binding(uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stageFlags);
  void add_binding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags);
};



#endif //DESCRIPTORLAYOUTBUILDER_H
