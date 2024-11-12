//
// Created by marti on 23/09/2024.
//

#ifndef DESCRIPTORALLOCATOR_H
#define DESCRIPTORALLOCATOR_H
#include <span>

#include "DescriptorLayoutBuilder.h"


class DescriptorAllocator {
public:

    struct PoolSizeRatio{
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;
    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    void clear_descriptors(VkDevice device);
    void destroy_pool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};



#endif //DESCRIPTORALLOCATOR_H
