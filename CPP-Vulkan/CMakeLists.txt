cmake_minimum_required(VERSION 3.25)
project(Beyond)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(vk-bootstrap CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED) # Vulkan::Headers, this loads vulkan.h
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED) # Sexy smexy memory allocator :3
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# import moltenvk if macOS
if (APPLE)
    find_library(MOLTENVK_LIBRARIES MoltenVK)
endif()

add_library(Beyond SHARED
        Window.cpp
        Window.h
        Graphics/VulkanBackend.cpp
        Graphics/VulkanBackend.h
        Graphics/vkinit.cpp
        Graphics/vkinit.h
        Graphics/vkutil.cpp
        Graphics/vkutil.h
        Graphics/Vulkan/Structs.cpp
        Graphics/Vulkan/Structs.h
        Graphics/Vulkan/DescriptorLayoutBuilder.cpp
        Graphics/Vulkan/DescriptorLayoutBuilder.h
        Graphics/Vulkan/DescriptorAllocator.cpp
        Graphics/Vulkan/DescriptorAllocator.h
        Graphics/Vulkan/vk_pipelines.cpp
        Graphics/Vulkan/vk_pipelines.h
        Graphics/Vulkan/ImGuiBackend.cpp
        Graphics/Vulkan/ImGuiBackend.h
        Graphics/Vulkan/VulkanContext.cpp
        Graphics/Vulkan/VulkanContext.h
        Graphics/Vulkan/PipelineBuilder.cpp
        Graphics/Vulkan/PipelineBuilder.h
        Graphics/Mesh.cpp
        Graphics/Mesh.h
        Graphics/Camera.cpp
        Graphics/Camera.h
        Graphics/ShaderManager.cpp
        Graphics/ShaderManager.h
        Graphics/Other/Transform.cpp
        Graphics/Other/Transform.h
        Graphics/Vulkan/MeshUtils.cpp
        Graphics/Vulkan/MeshUtils.h
        api.h
        api.cpp
        input.h
        input.cpp
)

# TODO: This project just became windows only, you can remove this if you want to support other platforms
# But I will now only support windows
# It will have remnants of other platforms, but they will be removed in the future
# Include FidelityFX SDK in ./FidelityFX/ffx-api/bin(or include)
target_link_directories(Beyond PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/FidelityFX/bin)
target_include_directories(Beyond PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/FidelityFX/include)


target_link_libraries(Beyond PRIVATE Vulkan::Headers vk-bootstrap::vk-bootstrap vk-bootstrap::vk-bootstrap-compiler-warnings glfw glm::glm Vulkan::Vulkan)
target_link_libraries(Beyond PRIVATE imgui::imgui)
target_link_libraries(Beyond PRIVATE GPUOpen::VulkanMemoryAllocator)
target_link_libraries(Beyond PRIVATE assimp::assimp)

if (APPLE)
    target_link_libraries(Beyond PRIVATE ${MOLTENVK_LIBRARIES})
endif()

target_include_directories(Beyond PUBLIC ${glm_INCLUDE_DIRS})

target_sources(Beyond PUBLIC api.h input.h)

set_target_properties(Beyond PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
)
