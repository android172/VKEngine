#include "renderer/vulkan/shaders/vulkan_shader.hpp"

VulkanShader::VulkanShader(
    const VulkanDevice* const device,
    const vk::AllocationCallbacks* const allocator
) : _device(device), _allocator(allocator) {}
VulkanShader::~VulkanShader() {}

// /////////////////////////////// //
// VULKAN SHADER PROTECTED METHODS //
// /////////////////////////////// //

vk::ShaderModule VulkanShader::create_shader_module(const std::vector<byte> code) const {
    vk::ShaderModuleCreateInfo create_info{};
    create_info.setCodeSize(code.size());
    create_info.setPCode(reinterpret_cast<const uint32*> (code.data()));
    return _device->handle.createShaderModule(create_info, _allocator);
}

void VulkanShader::create_pipeline(
    const std::vector<vk::PipelineShaderStageCreateInfo>& shader_stages,
    const vk::PipelineVertexInputStateCreateInfo& vertex_input_info,
    const vk::PipelineLayoutCreateInfo& layout_info,
    const vk::RenderPass render_pass,
    const vk::SampleCountFlagBits number_of_msaa_samples,
    const bool is_wire_frame
) {
    // === Input assembly ===
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info{};
    // What geometry will be drawn from the vertices. Possible values:
    //  - point list
    //  - line list
    //  - line strip
    //  - triangle list
    //  - triangle strip
    input_assembly_info.setTopology(vk::PrimitiveTopology::eTriangleList);
    // If enabled allows breakup of strip topologies (Used for line and triangle strips)
    input_assembly_info.setPrimitiveRestartEnable(false);

    // === Viewport and scissors ===
    // Dynamically allocated, so nothing goes here
    vk::PipelineViewportStateCreateInfo viewport_state_info{};
    viewport_state_info.setViewportCount(1);
    viewport_state_info.setScissorCount(1);

    // === Rasterizer ===
    vk::PipelineRasterizationStateCreateInfo rasterization_info{};
    // Clamp values beyond far/near planes instead of discarding them (feature required for enabling)
    rasterization_info.setDepthClampEnable(false);
    // Disable output to framebuffer (feature required for enabling)
    rasterization_info.setRasterizerDiscardEnable(false);
    // Determines how fragments are generated for geometry (feature required for changing)
    rasterization_info.setPolygonMode(
        is_wire_frame ? vk::PolygonMode::eLine : vk::PolygonMode::eFill
    );
    // Line thickness (feature required for values above 1)
    rasterization_info.setLineWidth(1.0f);
    // Triangle face we dont want to render
    rasterization_info.setCullMode(vk::CullModeFlagBits::eBack);
    // Set vertex order of front-facing triangles
    rasterization_info.setFrontFace(vk::FrontFace::eCounterClockwise);
    // Change depth information in some manner (used for shadow mapping)
    rasterization_info.setDepthBiasEnable(false);
    rasterization_info.setDepthBiasConstantFactor(0.0f);
    rasterization_info.setDepthBiasClamp(0.0f);
    rasterization_info.setDepthBiasSlopeFactor(0.0f);

    // === Multisampling ===
    vk::PipelineMultisampleStateCreateInfo multisampling_info{};
    // Number of samples used for multisampling
    multisampling_info.setRasterizationSamples(number_of_msaa_samples);
    // Is sample shading enabled
    multisampling_info.setSampleShadingEnable(true);
    // Min fraction of samples used for sample shading; closer to one is smoother
    multisampling_info.setMinSampleShading(0.2f);
    // Sample mask test
    multisampling_info.setPSampleMask(nullptr);
    multisampling_info.setAlphaToCoverageEnable(false);
    multisampling_info.setAlphaToOneEnable(false);

    // === Depth and stencil testing ===
    vk::PipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.setDepthTestEnable(true);                // Should depth testing be preformed
    // Should depth buffer be updated with new depth values (depth values of fragments that passed the depth test)
    depth_stencil.setDepthWriteEnable(true);
    depth_stencil.setDepthCompareOp(vk::CompareOp::eLess); // Comparison operation preformed for depth test
    depth_stencil.setDepthBoundsTestEnable(false);         // Should depth bods test be preformed
    depth_stencil.setMinDepthBounds(0.0f);                 // Minimum non-discarded fragment depth value
    depth_stencil.setMaxDepthBounds(1.0f);                 // Maximum non-discarded fragment depth value
    // Stencil buffer operations (here disabled)
    depth_stencil.setStencilTestEnable(false);
    depth_stencil.setFront({});
    depth_stencil.setBack({});

    // === Color blending ===
    std::array<vk::PipelineColorBlendAttachmentState, 1> color_blend_attachments;
    // Controls whether blending is enabled for the corresponding color attachment
    // If blending is not enabled, the source fragment’s color is passed through unmodified.
    color_blend_attachments[0].setBlendEnable(true);
    // Specifies which RGBA components are enabled for blending
    color_blend_attachments[0].setColorWriteMask(
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA
    );
    // Color blend options
    // Factor multiplied with source RGB
    color_blend_attachments[0].setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
    // Factor multiplied with destination RGB
    color_blend_attachments[0].setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
    // Blend operation used for calculating RGB of color attachment
    color_blend_attachments[0].setColorBlendOp(vk::BlendOp::eAdd);
    // Alpha blend option
    // Factor multiplied with source alpha
    color_blend_attachments[0].setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha);
    // Factor multiplied with destination alpha
    color_blend_attachments[0].setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
    // Blend operation used for calculating alpha of color attachment
    color_blend_attachments[0].setAlphaBlendOp(vk::BlendOp::eAdd);

    vk::PipelineColorBlendStateCreateInfo color_blend_state_info{};
    color_blend_state_info.setAttachments(color_blend_attachments);
    color_blend_state_info.setLogicOpEnable(false);        // Should Logical Op be applied
    color_blend_state_info.setLogicOp(vk::LogicOp::eCopy); // Which Op to apply if enabled

    // === Dynamic state ===
    std::array<vk::DynamicState, 3> dynamic_states = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        vk::DynamicState::eLineWidth
    };

    vk::PipelineDynamicStateCreateInfo dynamic_state_info{};
    // Pipeline information to be set dynamically at draw time
    dynamic_state_info.setDynamicStates(dynamic_states);

    // === Create pipeline layout ===
    try {
        _pipeline_layout = _device->handle.createPipelineLayout(layout_info, _allocator);
    } catch (vk::SystemError e) { Logger::fatal(e.what()); }

    // === Create pipeline object ===
    vk::GraphicsPipelineCreateInfo create_info{};
    // Programable pipeline stages
    create_info.setStages(shader_stages);
    // Fixed-function stages
    create_info.setPVertexInputState(&vertex_input_info);
    create_info.setPInputAssemblyState(&input_assembly_info);
    create_info.setPViewportState(&viewport_state_info);
    create_info.setPRasterizationState(&rasterization_info);
    create_info.setPMultisampleState(&multisampling_info);
    create_info.setPDepthStencilState(&depth_stencil);
    create_info.setPColorBlendState(&color_blend_state_info);
    create_info.setPDynamicState(&dynamic_state_info);
    create_info.setPTessellationState(nullptr); // TODO: Use tessellation
    // Pipeline layout handle
    create_info.setLayout(_pipeline_layout);
    // Render passes
    create_info.setRenderPass(render_pass);
    create_info.setSubpass(0); // The index of the subpass in the render pass where this pipeline will be used
    // Pipeline derivation
    create_info.setBasePipelineHandle(VK_NULL_HANDLE);
    create_info.setBasePipelineIndex(-1);

    try {
        auto result = _device->handle.createGraphicsPipeline(VK_NULL_HANDLE, create_info, _allocator);
        if (result.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to create graphics pipeline.");
        _pipeline = result.value;
    } catch (vk::SystemError e) { Logger::fatal(e.what()); }
}