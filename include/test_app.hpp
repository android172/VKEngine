#pragma once

#include "logger.hpp"

#include "renderer/renderer.hpp"

class TestApplication {
private:
    Platform::Surface* _app_surface = Platform::Surface::get_instance(800, 600, std::string(APP_NAME));
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
        _app_renderer.draw_frame(0);
    }

    _app_renderer.wait_for_shutdown();
}