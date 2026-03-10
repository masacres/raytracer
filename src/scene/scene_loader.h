#pragma once

#include "core/camera.h"
#include "geometry/hittable_list.h"
#include "renderer/renderer.h"
#include <optional>
#include <string>

struct SceneConfig {
    std::optional<Camera> camera;
    Renderer              renderer;
    HittableList          world;
    int                   width  = 800;
    int                   height = 450;
};

SceneConfig load_scene(const std::string& path);
