cmake_minimum_required(VERSION 3.29)
project(Beyond)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/marti/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

find_package(vk-bootstrap CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED) # Vulkan::Headers, this loads vulkan.h
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED) # Sexy smexy memory allocator :3
find_package(imgui CONFIG REQUIRED)

add_executable(Beyond main.cpp
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
)

target_link_libraries(Beyond PRIVATE Vulkan::Headers vk-bootstrap::vk-bootstrap vk-bootstrap::vk-bootstrap-compiler-warnings glfw glm::glm Vulkan::Vulkan)
target_link_libraries(Beyond PRIVATE imgui::imgui)
target_link_libraries(Beyond PRIVATE GPUOpen::VulkanMemoryAllocator)

target_include_directories(Beyond PRIVATE ${glm_INCLUDE_DIRS})


# Compiling shaders
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)

find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin $ENV{VULKAN_SDK}/Bin32)
message(STATUS "GLSL Validator found at: ${GLSL_VALIDATOR}")
message(STATUS "Compiling shaders from ${SHADER_DIR} to ${SHADER_OUTPUT_DIR}")

file(GLOB_RECURSE GLSL_SOURCE_FILES
        ${SHADER_DIR}/*.vert
        ${SHADER_DIR}/*.frag
        ${SHADER_DIR}/*.comp
        ${SHADER_DIR}/*.geom
        ${SHADER_DIR}/*.tesc
        ${SHADER_DIR}/*.tese
)

message(STATUS "Found ${GLSL_SOURCE_FILES}")

set(SPIRV_BINARY_FILES "")
# Create the directory for the output
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_OUTPUT_DIR}/${FILE_NAME}.spv")

    message(STATUS "Compiling ${GLSL} to ${SPIRV}")
    message(STATUS "Command: ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}")

    add_custom_command( DEPENDS
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
            VERBATIM
    )

    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()
add_custom_target(Shaders ALL DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(Beyond Shaders)
