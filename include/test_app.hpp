#pragma once

#include "platform/platform.hpp"
#include "pipeline.hpp"
#include "logger.hpp"

#include "renderer/renderer.hpp"

class TestApplication {
private:
    Platform::Surface* _app_surface = Platform::Surface::get_instance(800, 600, std::string(APP_NAME));
    Pipeline _app_pipeline{ "build/debug/shaders/simple_vertex_shader.vert.spv", "build/debug/shaders/simple_fragment_shader.frag.spv" };
    Renderer _app_renderer{ RendererBackendType::Vulkan, _app_surface };

public:
    TestApplication();
    ~TestApplication();

    void run();
};

TestApplication::TestApplication() {}

TestApplication::~TestApplication() {}

void TestApplication::run() {
    while (!_app_surface->should_close()) {
        _app_surface->process_events();
    }
}