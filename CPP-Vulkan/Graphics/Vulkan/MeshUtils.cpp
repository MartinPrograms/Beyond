//
// Created by marti on 29/09/2024.
//

#include "MeshUtils.h"

#include <cstring>

#include <sys/stat.h>
#include <string>
#include <fstream>

#include "VulkanContext.h"
#include "../vkinit.h"

// Import assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Graphics {
namespace Vulkan {
    MeshUtils::MeshUtils(VulkanContext &context): context(context) {

    }

    Graphics::Vulkan::GPUMeshBuffers MeshUtils::upload_mesh(std::span<uint32_t> indices,
                                                            std::span<Graphics::Vulkan::Vertex> vertices, VulkanContext &ctx) {

        const size_t vertexBufferSize = vertices.size() * sizeof(Vulkan::Vertex);
        const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

        Vulkan::GPUMeshBuffers newSurface;

        //create vertex buffer
        newSurface.vertexBuffer = Vulkan::AllocatedBuffer::create_buffer(ctx.allocator, vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

        //find the adress of the vertex buffer
        VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = newSurface.vertexBuffer.buffer };
        newSurface.vertexBufferDeviceAddress = vkGetBufferDeviceAddress(ctx.device, &deviceAdressInfo);

        //create index buffer
        newSurface.indexBuffer = Vulkan::AllocatedBuffer::create_buffer(ctx.allocator,indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

        Vulkan::AllocatedBuffer stagingBuffer = Vulkan::AllocatedBuffer::create_buffer(ctx.allocator, vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU);

        void* data;
        VkResult result = vmaMapMemory(ctx.allocator, stagingBuffer.allocation, &data);
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
            }, ctx);
            vmaUnmapMemory(ctx.allocator, stagingBuffer.allocation);

            stagingBuffer.destroy(ctx.allocator);
        } else {
            // Handle error
            std::cout << "Failed to map memory!" << std::endl;
        }

        // Don't forget to unmap the memory after you're done
        std::cout << "Unmapped memory!" << std::endl;
        return newSurface;

    }

    void MeshUtils::immediate_submit(std::function<void(VkCommandBuffer cmd)> &&function, VulkanContext &context) {

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

    Mesh::Mesh MeshUtils::loadMesh(std::string path, int index, std::string pipelineName) {
        // Check if the file exists
        struct stat buffer;
        if (stat(path.c_str(), &buffer) != 0) {
            std::cout << "File " << path << " does not exist!" << std::endl;
    // Throw an exception here
            throw std::runtime_error("File does not exist!");
        }
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace |
            aiProcess_GenUVCoords |
            aiProcess_OptimizeMeshes |
            aiProcess_OptimizeGraph);

        // We only get the index-th mesh

        auto mesh = scene->mMeshes[index];

        // TODO: Read out texture data and shader variables for pbr and stuff

        std::vector<Vulkan::Vertex> vertices;
        std::vector<uint32_t> indices;

        for (int i = 0; i < mesh->mNumVertices; i++) {
            Vulkan::Vertex vertex;

            auto pos = mesh->mVertices[i];
            vertex.position = { pos.x, pos.y, pos.z };

            auto uv = mesh->mTextureCoords[0][i];
            vertex.uv_x = uv.x;
            vertex.uv_y = uv.y;

            auto normal = mesh->mNormals[i];
            vertex.normal = { normal.x, normal.y, normal.z };

            auto tangent = mesh->mTangents[i];
            vertex.tangent = { tangent.x, tangent.y, tangent.z };

            auto bitangent = mesh->mBitangents[i];
            vertex.bitangent = { bitangent.x, bitangent.y, bitangent.z };

            vertices.push_back(vertex);
        }

        for (int i = 0; i < mesh->mNumFaces; i++) {
            auto face = mesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        auto vkmesh = Mesh::Mesh(pipelineName);

        vkmesh.setVertices(vertices.data(), vertices.size());
        vkmesh.setIndices(indices.data(), indices.size());

        vkmesh.createBuffers(context);

        return vkmesh;
    }

    Mesh::Mesh MeshUtils::loadMesh(std::span<uint32_t> indices, std::span<Graphics::Vulkan::Vertex> vertices, std::string pipelineName) {
        auto vkmesh = Mesh::Mesh(pipelineName);

        vkmesh.setVertices(vertices.data(), vertices.size());
        vkmesh.setIndices(indices.data(), indices.size());

        vkmesh.createBuffers(context);

        return vkmesh;
    }
} // Vulkan
} // Graphics