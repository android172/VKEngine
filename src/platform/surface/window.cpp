#include "platform/platform.hpp"
#if PLATFORM == WINDOWS32 || PLATFORM == LINUX

#include "platform/surface/window.hpp"

Platform::Surface* Platform::Surface::get_instance(uint32 width, uint32 height, std::string name) {
    return new Window(width, height, name);
}

const std::vector<const char*> Platform::get_required_vulkan_extensions() {
    uint32_t count;
    auto extensions = glfwGetRequiredInstanceExtensions(&count);
    return std::vector(extensions, extensions + count);
}


Window::Window(int32 width, int32 height, std::string name) : _width(width), _height(height), _name(name) {
    // initialize GLFW with parameters
    glfwInit();                                     // Initialize GLFW
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   // We dont want a OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);     // Window resize disabled TODO: enable

    // Create window :: width, height, window name, monitor, share (OpenGL only)
    _window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
}

Window::~Window() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

vk::SurfaceKHR Window::get_vulkan_surface(vk::Instance& vulkan_instance, vk::AllocationCallbacks* allocator) {
    vk::SurfaceKHR vulkan_surface;
    VkResult result = glfwCreateWindowSurface(
        vulkan_instance, _window, (VkAllocationCallbacks*) allocator, (VkSurfaceKHR*) &vulkan_surface);
    if (result != VK_SUCCESS)
        throw "Failed to create window surface.";
    return vulkan_surface;
}

void Window::process_events() {
    glfwPollEvents();
    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        exit(EXIT_SUCCESS);
    }
}

#endif