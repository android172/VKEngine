/*
#pragma once

#include "renderer/renderer_backend.hpp"
#include "renderer/vulkan/vulkan_structures.hpp"
#include "vulkan_command_pool.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_image.hpp"

class VulkanBackend : public RendererBackend {
private:
    // TODO: Custom allocator
    vk::AllocationCallbacks* _allocator = nullptr;

    vk::Instance _vulkan_instance;
    vk::DebugUtilsMessengerEXT _debug_messenger;

    const std::vector<const char*> _validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    void create_vulkan_instance();
    void setup_debug_messenger();

    bool all_validation_layers_are_available();
    vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_create_info();

    // Surface
    Platform::Surface* _surface;
    vk::SurfaceKHR _vulkan_surface;

    // Buffer
    void create_buffer(
        vk::DeviceSize size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        vk::Buffer& buffer, vk::DeviceMemory& buffer_memory
    );
    void copy_buffer(
        vk::Buffer source_buffer,
        vk::Buffer destination_buffer,
        vk::DeviceSize size
    );
    void copy_buffer_to_image(
        vk::Buffer buffer,
        vk::Image image,
        uint32 width,
        uint32 height
    );

    // DEVICE CODE
    VulkanDevice _device;

    void create_device();

    // SWAPCHAIN CODE
    vk::SwapchainKHR _swapchain;
    vk::Format _swapchain_format;
    vk::Extent2D _swapchain_extent;
    std::vector<vk::ImageView> _swapchain_image_views;
    std::vector<vk::Framebuffer> _swapchain_framebuffers;
    uint32 current_frame = 0;

    void create_swapchain();
    void recreate_swapchain();
    void cleanup_swapchain();
    void create_framebuffers();
    void present_swapchain();

    // TODO: TEMP PIPELINE CODE
    vk::RenderPass _render_pass;
    vk::PipelineLayout _pipeline_layout;
    vk::Pipeline _graphics_pipeline;

    void create_render_pass();
    void create_pipeline();

    vk::ShaderModule create_shader_module(const std::vector<byte>& code);

    // TODO: TEMP COMMAND CODE
    VulkanCommandPool* _command_pool;
    std::vector<vk::CommandBuffer> _command_buffers;

    void record_command_buffer(vk::CommandBuffer command_buffer, uint32 image_index);

    // TODO: TEMP SYNCHRONIZATION CODE
    std::vector<vk::Semaphore> _semaphores_image_available;
    std::vector<vk::Semaphore> _semaphores_render_finished;
    std::vector<vk::Fence> _fences_in_flight;

    void create_sync_objects();

    // TODO: TEMP VERTEX BUFFER CODE
    vk::Buffer _vertex_buffer;
    vk::DeviceMemory _vertex_buffer_memory;

    void create_vertex_buffer();

    // TODO: TEMP INDEX BUFFER CODE
    vk::Buffer _index_buffer;
    vk::DeviceMemory _index_buffer_memory;

    void create_index_buffer();

    // TODO: TEMP UNIFORM CODE
    vk::DescriptorSetLayout _descriptor_set_layout;
    vk::DescriptorPool _descriptor_pool;
    std::vector<vk::DescriptorSet> _descriptor_sets;

    std::vector<vk::Buffer> _uniform_buffers;
    std::vector<vk::DeviceMemory> _uniform_buffers_memory;

    void create_descriptor_set_layout();
    void create_descriptor_pool();
    void create_descriptor_sets();
    void create_uniform_buffers();
    void update_uniform_buffer(uint32 current_image);

    // TODO: TEMP IMAGE CODE
    VulkanImage _texture_image = { &_device, _allocator };
    vk::Sampler _texture_sampler;

    void create_texture_image();
    void create_texture_sampler();

    // TODO: TEMP DEPTH BUFFER CODE
    VulkanImage _depth_image = { &_device, _allocator };

    void create_depth_resources();
    vk::Format find_depth_format();

    // TODO: TEMP MODEL LOADING CODE
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;

    void load_model();

    // TODO: TEMP MIPMAP CODE
    uint32 _mip_levels;

    void generate_mipmaps(vk::Image image, vk::Format format, uint32 width, uint32 height, uint32 mip_levels);

    // TODO: TEMP MSAA CODE
    vk::SampleCountFlagBits _msaa_samples = vk::SampleCountFlagBits::e1;
    VulkanImage _color_image = { &_device, _allocator };

    void create_color_resource();

public:
    VulkanBackend(Platform::Surface* surface);
    ~VulkanBackend();

    void resized(uint32 width, uint32 height);
    bool begin_frame(float32 delta_time);
    bool end_frame(float32 delta_time);

    void wait_for_shutdown() {
        _device.handle.waitIdle();
    }

    // TODO: TEMP DRAW CODE
    void draw_frame();
};
*/