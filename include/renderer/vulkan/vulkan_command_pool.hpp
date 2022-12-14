#pragma once

#include <vulkan/vulkan.hpp>

#include "vulkan_types.hpp"
#include "logger.hpp"

/**
 * @brief Vulkan command pool. This class is used for spawning and managing
 * individual Vulkan Command Buffers and issuing single-time commands.
 */
class VulkanCommandPool {
  public:

    /**
     * @brief Construct a new Vulkan Command Pool object
     *
     * @param device Vulkan device reference
     * @param allocator Allocation callback used
     * @param queue Reference to a Queue to which the command pool commands will
     * be send
     * @param queue_index Index of referenced pool
     */
    VulkanCommandPool(
        const vk::Device* const              device,
        const vk::AllocationCallbacks* const allocator,
        const vk::Queue* const               queue,
        const uint32                         queue_index
    );
    ~VulkanCommandPool();

    /// @brief Allocate vulkan command buffer from the pool
    /// @param primary Specify whether the created buffer is a primary buffer
    /// @returns Created and allocated buffer
    vk::CommandBuffer allocate_command_buffer(const bool primary = true) const;
    /// @brief Allocate vulkan command buffers from the pool
    /// @param primary Specify whether created buffers are primary buffers
    /// @param count Total buffer count
    /// @returns Created and allocated buffers
    Vector<vk::CommandBuffer> allocate_command_buffers(
        const uint32 count, const bool primary = true
    ) const;
    /// @brief Allocate managed vulkan command buffer from the pool. Creates one
    /// per frame
    /// @param primary Specify whether created buffers are primary buffers
    /// @returns Created and allocated VulkanBuffers
    VulkanCommandBuffer* allocate_managed_command_buffer(
        const bool primary = true
    ) const;

    /// @brief Returns command buffer to the pool
    /// @param command_buffer Buffer to free
    void free_command_buffer(vk::CommandBuffer& command_buffer) const;
    /// @brief Returns command buffers to the pool
    /// @param command_buffers Buffers to free
    void free_command_buffers(Vector<vk::CommandBuffer>& command_buffers) const;

    /// @brief Allocate and begin a single use buffer
    /// @returns Created buffer
    vk::CommandBuffer begin_single_time_commands() const;
    /// @brief End and free a single use buffer
    /// @param command_buffer Buffer to free
    void end_single_time_commands(vk::CommandBuffer& command_buffer) const;

  private:
    const vk::Device*                    _device;
    const vk::AllocationCallbacks* const _allocator;
    const vk::Queue* const               _queue;

    vk::CommandPool _command_pool;
};