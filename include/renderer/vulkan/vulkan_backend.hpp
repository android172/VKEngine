#pragma once

#include "renderer/renderer_backend.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_render_pass.hpp"
#include "shaders/vulkan_object_shader.hpp"
#include "../renderer_types.hpp"

#include "resources/texture.hpp"

class VulkanBackend : public RendererBackend {
public:
    VulkanBackend(Platform::Surface* surface);
    ~VulkanBackend();

    void resized(const uint32 width, const uint32 height);
    bool begin_frame(const float32 delta_time);
    bool end_frame(const float32 delta_time);
    void update_global_uniform_buffer_state(
        const glm::mat4 projection,
        const glm::mat4 view,
        const glm::vec3 view_position,
        const glm::vec4 ambient_color,
        const int32 mode
    );

    void create_texture(Texture* texture);
    void destroy_texture(Texture* texture);

private:
    // TODO: Custom allocator
    const vk::AllocationCallbacks* const _allocator = nullptr;

    vk::Instance _vulkan_instance;
    vk::DebugUtilsMessengerEXT _debug_messenger;

    vk::Instance create_vulkan_instance() const;
    vk::DebugUtilsMessengerEXT setup_debug_messenger() const;

    bool all_validation_layers_are_available() const;
    vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_create_info() const;

    // Surface
    vk::SurfaceKHR _vulkan_surface;

    // Synchronization objects
    std::vector<vk::Semaphore> _semaphores_image_available;
    std::vector<vk::Semaphore> _semaphores_render_finished;
    std::vector<vk::Fence> _fences_in_flight;

    void create_sync_objects();

    // DEVICE CODE
    VulkanDevice* _device;

    // SWAPCHAIN CODE
    VulkanSwapchain* _swapchain;
    uint32 _current_frame = 0;

    // RENDER PASS
    VulkanRenderPass* _render_pass;

    // OBJECT SHADER
    VulkanObjectShader* _object_shader;

    // TODO: TEMP COMMAND CODE
    VulkanCommandPool* _command_pool;
    std::vector<vk::CommandBuffer> _command_buffers;

    void record_command_buffer(vk::CommandBuffer command_buffer, uint32 image_index);

    // TODO: TEMP VERTEX BUFFER CODE
    VulkanBuffer* _vertex_buffer;

    void create_vertex_buffer();

    // TODO: TEMP INDEX BUFFER CODE
    VulkanBuffer* _index_buffer;

    void create_index_buffer();

    // TODO: TEMP UNIFORM CODE
    std::vector<VulkanBuffer*> _uniform_buffers;

    void create_descriptor_sets();
    void create_uniform_buffers();

    // TODO: TEMP IMAGE CODE
    VulkanImage* _texture_image;
    vk::Sampler _texture_sampler;

    void create_texture_image();
    void create_texture_sampler();

    // TODO: TEMP MODEL LOADING CODE
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;

    void load_model();

    // TODO: TEMP MIPMAP CODE
    uint32 _mip_levels;

    void generate_mipmaps(vk::Image image, vk::Format format, uint32 width, uint32 height, uint32 mip_levels);
};