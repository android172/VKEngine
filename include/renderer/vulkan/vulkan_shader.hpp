#pragma once

#include "vulkan_render_pass.hpp"
#include "vulkan_managed_buffer.hpp"
#include "vulkan_settings.hpp"
#include "resources/shader.hpp"

class ResourceSystem;

/**
 * @brief Configuration for descriptor set. Contains layout and binding
 * description.
 */
struct VulkanDescriptorSetConfig {
    vk::DescriptorSetLayout                layout;
    Vector<vk::DescriptorSetLayoutBinding> bindings;
};

/**
 * @brief An instance-level shader state.
 */
struct VulkanInstanceState {
    uint64 offset;

    Vector<Texture*> instance_textures;
    std::array<vk::DescriptorSet, VulkanSettings::max_frames_in_flight>
        descriptor_set;
    std::array<std::optional<uint32>, VulkanSettings::max_frames_in_flight>
        descriptor_set_ids;
};

/**
 * @brief  Vulkan implementation of generic shader. This uses a set of inputs
 * and parameters, as well as the shader programs contained in SPIR-V files to
 * construct a shader for use in rendering.
 */
class VulkanShader : public Shader {
  public:
    /**
     * @brief Construct a new Vulkan Shader object.
     * @param config Shader configuration used
     * @param device Device on which the shader will be used
     * @param allocator Custom allocation callback
     * @param render_pass Pointer to the render pass used by this shader
     * off disk
     */
    VulkanShader(
        const ShaderConfig                   config,
        const VulkanDevice* const            device,
        const vk::AllocationCallbacks* const allocator,
        const VulkanRenderPass* const        render_pass,
        const VulkanCommandBuffer* const     command_buffer
    );
    ~VulkanShader();

    void use() override;
    void bind_globals() override;
    void bind_instance(const uint32 id) override;
    void apply_global() override;
    void apply_instance() override;

    uint32 acquire_instance_resources() override;
    void   release_instance_resources(uint32 instance_id) override;

    const static uint32 max_descriptor_sets = 1024; // TODO: Maybe make dynamic

  protected:
    bool set_uniform(const uint16 id, void* value) override;

  private:
    const VulkanDevice*                  _device;
    const vk::AllocationCallbacks* const _allocator;
    const VulkanRenderPass*              _render_pass;
    const VulkanCommandBuffer* const     _command_buffer;

    // Pipeline
    vk::Pipeline       _pipeline;
    vk::PipelineLayout _pipeline_layout;

    // Descriptors
    vk::DescriptorPool                 _descriptor_pool;
    Vector<VulkanDescriptorSetConfig*> _descriptor_set_configs;
    Vector<vk::DescriptorSet>          _global_descriptor_sets;

    // Instances
    Vector<VulkanInstanceState*> _instance_states;

    // Buffers
    VulkanManagedBuffer* _uniform_buffer;
    uint64               _uniform_buffer_offset;

    // Constants
    const uint32 _desc_set_index_global   = 0;
    const uint32 _desc_set_index_instance = 1;

    const uint8 _bind_index_ubo     = 0;
    const uint8 _bind_index_sampler = 1;

    vk::ShaderModule create_shader_module(
        const vk::ShaderStageFlagBits shader_stage
    ) const;
    Vector<vk::PipelineShaderStageCreateInfo> compute_stage_infos(
        const Vector<vk::ShaderStageFlagBits>& shader_stages
    ) const;
    Vector<vk::VertexInputAttributeDescription> compute_attributes() const;
    Vector<VulkanDescriptorSetConfig*>          compute_uniforms(
                 const Vector<vk::ShaderStageFlagBits>& shader_stages
             ) const;

    void create_pipeline(
        const Vector<vk::PipelineShaderStageCreateInfo>& shader_stages,
        const vk::PipelineVertexInputStateCreateInfo&    vertex_input_info,
        const bool                                       is_wire_frame = false
    );

    Vector<vk::DescriptorImageInfo>& get_image_infos(
        const Vector<Texture*>& textures
    ) const;
};