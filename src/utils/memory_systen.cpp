#include "memory_system.hpp"

#include "resources/material.hpp"

static_assert(
    sizeof(MEMORY_TAG_TYPE) <= MEMORY_PADDING || MEMORY_PADDING >= 8,
    "Memory padding must be at least 8."
);
static_assert(
    (uint64) MemoryTag::MAX_TAGS <
        ((uint64) 1 << (sizeof(MEMORY_TAG_TYPE) * 8 - 4)),
    "No enough space to store all tags. (NOTE: Last 4 bits of enum binary "
    "representation are used to recognize custom allocation)"
);

Allocator** MemorySystem::_allocator_map =
    MemorySystem::initialize_allocator_map();

Allocator** MemorySystem::initialize_allocator_map() {
    Allocator** allocator_map =
        new Allocator*[(MEMORY_TAG_TYPE) MemoryTag::MAX_TAGS]();

    // Define used allocators
    CAllocator*        unknown_allocator = new CAllocator();
    StackAllocator*    temp_allocator    = new StackAllocator(1024 * 1024);
    FreeListAllocator* general_allocator = new FreeListAllocator(
        1024 * 1024, FreeListAllocator::PlacementPolicy::FindFirst
    );
    FreeListAllocator* gpu_data_allocator = new FreeListAllocator(
        1024 * 1024, FreeListAllocator::PlacementPolicy::FindFirst
    );
    FreeListAllocator* resource_allocator = new FreeListAllocator(
        1024 * 1024, FreeListAllocator::PlacementPolicy::FindFirst
    );
    LinearAllocator* init_allocator = new LinearAllocator(1024 * 1024);

    // Pools
    uint64 max_texture_count  = 1024;
    uint64 max_material_count = 1024;

    uint64 texture_size  = sizeof(Texture) + MEMORY_PADDING;
    uint64 material_size = sizeof(Material) + MEMORY_PADDING;

    PoolAllocator* texture_pool =
        new PoolAllocator(max_texture_count * texture_size, texture_size);
    PoolAllocator* material_pool =
        new PoolAllocator(max_material_count * material_size, material_size);

    // Initialize allocators
    unknown_allocator->init();
    temp_allocator->init();
    general_allocator->init();
    gpu_data_allocator->init();
    resource_allocator->init();
    init_allocator->init();
    texture_pool->init();
    material_pool->init();

    // Assign allocators
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Unknown] = unknown_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Temp]    = temp_allocator;

    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Array]       = general_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::List]        = general_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Map]         = general_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Set]         = general_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::String]      = general_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Callback]    = general_allocator;
    // Engine
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Application] = init_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Surface]     = init_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::System]      = init_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Renderer]    = init_allocator;
    // GPU local
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::GPUTexture] = gpu_data_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::GPUBuffer]  = gpu_data_allocator;
    // Resources
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Resource]   = resource_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Texture]    = texture_pool;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::MaterialInstance] =
        material_pool;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Geometry]   = unknown_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Shader]     = resource_allocator;
    // Game
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Game]       = init_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Job]        = unknown_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Transform]  = unknown_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Entity]     = unknown_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::EntityNode] = unknown_allocator;
    allocator_map[(MEMORY_TAG_TYPE) MemoryTag::Scene]      = unknown_allocator;

    return allocator_map;
}

// New
void* operator new(std::size_t size, MemoryTag tag) {
    void* full_ptr = MemorySystem::allocate(size + MEMORY_PADDING, tag);
    void* data_ptr = (void*) ((uint64) full_ptr + MEMORY_PADDING);

    MEMORY_TAG_TYPE tag_data = ((MEMORY_TAG_TYPE) tag) << 4;
    tag_data |= 15; // 1111

    MemoryTag* header_ptr = (MemoryTag*) full_ptr;
    *header_ptr           = (MemoryTag) tag_data;

    return data_ptr;
}
void* operator new[](std::size_t size, const MemoryTag tag) {
    return operator new(size, tag);
}

// Delete
void operator delete(void* p) noexcept {
    if (p == nullptr) return;
    MEMORY_TAG_TYPE* tag_ptr = (MEMORY_TAG_TYPE*) ((uint64) p - MEMORY_PADDING);
    MemoryTag        tag     = (MemoryTag) ((*tag_ptr) >> 4);

    // If 4 bytes before p are 1111 we are using custom allocator
    uint8* tag_type_ptr = (uint8*) tag_ptr;
    uint8  tag_type     = *tag_type_ptr << 4;

    // 240 = 11110000
    if (tag_type != 240) free(p);
    else {
        *tag_type_ptr &= 240;
        MemorySystem::deallocate(tag_ptr, tag);
    }
}