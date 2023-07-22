#pragma once

#include "math_libs.hpp"
#include "serialization/serializer.hpp"

// -----------------------------------------------------------------------------
// Vertex
// -----------------------------------------------------------------------------
/**
 * @brief Vertex in 3D space
 *
 */
struct Vertex3D {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec4 color;
    glm::vec2 texture_coord;

    Vertex3D() {}
    Vertex3D(
        const glm::vec3 position,
        const glm::vec3 normal,
        const glm::vec4 tangent,
        const glm::vec4 color,
        const glm::vec2 texture_coord
    )
        : position(position), normal(normal), tangent(tangent), color(color),
          texture_coord(texture_coord) {}
    ~Vertex3D() {}

    bool operator==(const Vertex3D& other) const {
        return other.position == position && //
               other.normal == normal &&     //
               other.tangent == tangent &&   //
               other.color == color &&       //
               other.texture_coord == texture_coord;
    }
};

template<>
inline String serialize_object<Vertex3D>(
    const Vertex3D& obj, const Serializer* const serializer
) {
    return serializer->serialize(
        obj.position, obj.normal, obj.tangent, obj.color, obj.texture_coord
    );
}
template<>
inline Result<uint32, RuntimeError> deserialize_object<Vertex3D>(
    Vertex3D&               obj,
    const Serializer* const serializer,
    const String&           data,
    const uint32            from_pos
) {
    return serializer->deserialize(
        data,
        from_pos,
        obj.position,
        obj.normal,
        obj.tangent,
        obj.color,
        obj.texture_coord
    );
}

/**
 * @brief Vertex in 2D plane
 *
 */
struct Vertex2D {
    glm::vec2 position;
    glm::vec2 texture_coord;

    Vertex2D() {}
    Vertex2D(const glm::vec2 position, const glm::vec2 texture_coord)
        : position(position), texture_coord(texture_coord) {}
    ~Vertex2D() {}

    bool operator==(const Vertex2D& other) const {
        return other.position == position &&
               other.texture_coord == texture_coord;
    }
};

template<>
inline String serialize_object<Vertex2D>(
    const Vertex2D& obj, const Serializer* const serializer
) {
    return serializer->serialize(obj.position, obj.texture_coord);
}
template<>
inline Result<uint32, RuntimeError> deserialize_object<Vertex2D>(
    Vertex2D&               obj,
    const Serializer* const serializer,
    const String&           data,
    const uint32            from_pos
) {
    return serializer->deserialize(
        data, from_pos, obj.position, obj.texture_coord
    );
}

typedef Vertex3D Vertex;

namespace std {
template<>
struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const {
        return hash<glm::vec3>()(vertex.position) ^
               hash<glm::vec2>()(vertex.texture_coord);
    }
};
} // namespace std

// -----------------------------------------------------------------------------
// Render data
// -----------------------------------------------------------------------------
/**
 * @brief List of builtin render passes
 */
enum BuiltinRenderPass : uint8 { World = 0x1, UI = 0x2 };

/**
 * @brief Render debug view modes
 */
enum DebugViewMode : uint8 { Default, Lighting, Normals };

class Geometry;

/**
 * @brief Geometry render packet
 */
struct GeometryRenderData {
    Geometry* geometry;
};