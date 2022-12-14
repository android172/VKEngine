#pragma once

#include "texture.hpp"
#include "unordered_map.hpp"

class ResourceSystem;
class TextureSystem;

/// @brief Supported shader attribute types
enum class ShaderAttributeType : uint8 {
    float32,
    vec2,
    vec3,
    vec4,
    int8,
    int16,
    int32,
    uint8,
    uint16,
    uint32,
    COUNT
};
/// @brief Supported uniform types
enum class ShaderUniformType : uint8 {
    float32,
    vec2,
    vec3,
    vec4,
    int8,
    int16,
    int32,
    uint8,
    uint16,
    uint32,
    matrix4,
    sampler,
    custom
};

/// @brief Shader stages available
enum class ShaderStage : uint8 {
    Vertex   = 0x1,
    Geometry = 0x2,
    Fragment = 0x4,
    Compute  = 0x8
};

/// @brief Shader scope
enum class ShaderScope : uint8 { Global, Instance, Local };

/// @brief Structure containing all attribute relevant data
struct ShaderAttribute {
    String              name;
    uint32              size;
    ShaderAttributeType type;
};
/// @brief Shader uniform configuration
struct ShaderUniformConfig {
    String            name;
    uint8             size;
    uint32            location;
    ShaderUniformType type;
    ShaderScope       scope;
};
/// @brief Structure containing all uniform relevant data
struct ShaderUniform {
    uint64            offset;
    uint16            size;
    uint16            location;
    uint16            index;
    uint8             set_index;
    ShaderScope       scope;
    ShaderUniformType type;
};

/**
 * @brief Shader configuration resource. Usually this resource is loaded from a
 * .shadercfg file.
 */
class ShaderConfig : public Resource {
  public:
    ResourceSystem*                   resource_system = nullptr;
    TextureSystem*                    texture_system  = nullptr;
    const String                      render_pass_name;
    const uint8                       shader_stages;
    const Vector<ShaderAttribute>     attributes;
    const Vector<ShaderUniformConfig> uniforms;
    const bool                        use_instances;
    const bool                        use_locals;

    ShaderConfig(
        const String&                      name,
        const String&                      render_pass_name,
        const uint8                        shader_stages,
        const Vector<ShaderAttribute>&     attributes,
        const Vector<ShaderUniformConfig>& uniforms,
        const bool                         use_instances,
        const bool                         use_locals
    )
        : Resource(name), render_pass_name(render_pass_name),
          shader_stages(shader_stages), attributes(attributes),
          uniforms(uniforms), use_instances(use_instances),
          use_locals(use_locals) {}
    ~ShaderConfig() {}
};

/// @brief Push constant range description
struct PushConstantRange {
    uint64 offset;
    uint64 size;
};

/**
 * @brief Frontend (API agnostic) representation of a shader.
 */
class Shader {
  public:
    Shader(const ShaderConfig config);
    virtual ~Shader();

    /**
     * @brief Use this shader
     */
    virtual void use();

    /**
     * @brief Binds shader to global scope. Must be done before setting global
     * level uniforms.
     */
    virtual void bind_globals();
    /**
     * @brief Binds specific shader instance for use. Must be done before the
     * setting of instance level uniforms.
     * @param id
     */
    virtual void bind_instance(const uint32 id);

    /**
     * @brief Apply set global uniforms
     */
    virtual void apply_global();
    /**
     * @brief Apply set instance uniforms
     */
    virtual void apply_instance();

    /**
     * @brief Acquires resources required for initialization of a shader
     * instance
     * @return uint32 instance id
     */
    virtual uint32 acquire_instance_resources();
    /**
     * @brief Release previously acquired instance resources
     * @param instance_id Id of instance to be released
     */
    virtual void   release_instance_resources(uint32 instance_id);

    /**
     * @brief Get the the index of a requested uniform
     *
     * @param name The name of the uniform to search for
     * @returns Uniform index if found
     * @throws InvalidArgument exception if no uniform is found
     */
    Result<uint16, InvalidArgument> get_uniform_index(const String name);

    /**
     * @brief Set the uniform value by uniform name
     *
     * @tparam T Value type
     * @param name Uniform name
     * @param value Value we wish to set
     * @throws InvalidArgument exception if no uniform is found
     */
    template<typename T>
    Result<void, InvalidArgument> set_uniform(
        const String name, const T* value
    ) {
        // Get id
        auto id = get_uniform_index(name);
        if (id.has_error())
            return Failure(InvalidArgument(String::build(
                "Couldn't set required uniform \"",
                name,
                "\" because no such uniform exists."
            )));
        // Set uniform with id
        return set_uniform<T>(id.value(), value);
    }
    /**
     * @brief Set the uniform value by uniform id
     *
     * @tparam T Value type
     * @param name Uniform id
     * @param value Value we wish to set
     * @throws InvalidArgument exception if no uniform is found
     */
    template<typename T>
    Result<void, InvalidArgument> set_uniform(const uint16 id, const T* value) {
        if (id < 0 || id >= _uniforms.size())
            return Failure(InvalidArgument(String::build(
                "Couldn't set required uniform id=",
                id,
                " because no such uniform exists."
            )));
        set_uniform(id, (void*) value);
        return {};
    }

    /**
     * @brief Set the sampler texture by sampler name
     *
     * @param name Sampler name
     * @param texture Texture the sampler will use
     * @throws InvalidArgument exception if no sampler is found
     */
    Result<void, InvalidArgument> set_sampler(
        const String name, const Texture* const texture
    );
    /**
     * @brief Set the sampler texture by sampler id
     *
     * @param name Sampler id
     * @param texture Texture the sampler will use
     * @throws InvalidArgument exception if no sampler is found
     */
    Result<void, InvalidArgument> set_sampler(
        const uint16 id, const Texture* const texture
    );

    const static uint32 max_name_length    = 256;
    const static uint32 max_instance_count = 1024;

  protected:
    TextureSystem*  _texture_system;
    ResourceSystem* _resource_system;

    String _name;
    bool   _use_instances;
    bool   _use_locals;
    uint64 _required_ubo_alignment;

    // Currently bound
    ShaderScope _bound_scope;
    uint32      _bound_instance_id;
    uint32      _bound_ubo_offset;

    // Attributes
    Vector<ShaderAttribute> _attributes {};
    uint16                  _attribute_stride = 0;

    // Uniforms
    UnorderedMap<String, uint32> _uniforms_hash {};
    Vector<ShaderUniform>        _uniforms {};

    // Global uniforms
    uint64 _global_ubo_size   = 0;
    uint64 _global_ubo_stride = 0;
    uint64 _global_ubo_offset = 0;

    // Instance uniforms
    uint64 _ubo_size   = 0;
    uint64 _ubo_stride = 0;

    // Local uniforms
    uint64                    _push_constant_size   = 0;
    uint64                    _push_constant_stride = 128;
    Vector<PushConstantRange> _push_constant_ranges {};

    // Textures
    Vector<Texture*> _global_textures {};
    uint8            _instance_texture_count;

    virtual bool set_uniform(const uint16 id, void* value);

  private:
    void add_sampler(const ShaderUniformConfig& config);
    void add_uniform(
        const ShaderUniformConfig&  config,
        const std::optional<uint32> location = std::optional<uint32>()
    );
};
