#pragma once

#include "vulkan_types.hpp"
#include "logger.hpp"
#include "property.hpp"

/**
 * @brief Vulkan representation of device. Almost every vulkan object requires a
 * device on which it is created to be specified. This class provides device
 * related functionalities and info.
 */
class VulkanDevice {
  public:
    /// @brief Vulkan handle to a logical device.
    Property<vk::Device> handle {
        GET { return _handle; }
    };
    /// @brief Vulkan physical device info.
    Property<PhysicalDeviceInfo> info {
        GET { return _info; }
    };

    /**
     * @brief Construct a new Vulkan Device object
     *
     * @param vulkan_instance Reference to Vulkan backend instance
     * @param vulkan_surface Reference to Vulkan surface object
     * @param allocator Allocation callback used
     */
    VulkanDevice(
        const vk::Instance&                  vulkan_instance,
        const vk::SurfaceKHR&                vulkan_surface,
        const vk::AllocationCallbacks* const allocator
    );
    ~VulkanDevice();

    /// @brief Searches through the list of available memory types.
    /// @param type_filter Memory types to filter by.
    /// @param properties Memory properties that need to be satisfied.
    /// @return Index of a memory type satisfying above specified conditions.
    /// @throws runtime_error If no suitable memory type is found.
    Result<uint32, RuntimeError> find_memory_type(
        const uint32 type_filter, const vk::MemoryPropertyFlags properties
    ) const;

    // Queue families
    QueueFamilyIndices queue_family_indices;
    vk::Queue          graphics_queue;
    vk::Queue          presentation_queue;
    vk::Queue          transfer_queue;
    vk::Queue          compute_queue;

  private:
    const vk::AllocationCallbacks* const _allocator = nullptr;

    vk::Device         _handle;
    PhysicalDeviceInfo _info;

    vk::PhysicalDevice pick_physical_device(
        const vk::Instance&   vulkan_instance,
        const vk::SurfaceKHR& vulkan_surface
    ) const;
    vk::Device create_logical_device(const vk::PhysicalDevice physical_device
    ) const;

    PhysicalDeviceInfo get_physical_device_info(
        const vk::PhysicalDevice physical_device
    ) const;
    int32 rate_device_suitability(
        const vk::PhysicalDevice& device, const vk::SurfaceKHR& vulkan_surface
    ) const;
    QueueFamilyIndices find_queue_families(
        const vk::PhysicalDevice& device, const vk::SurfaceKHR& vulkan_surface
    ) const;
    SwapchainSupportDetails query_swapchain_support_details(
        const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface
    ) const;
};