#include "scene/scene_loader.h"
#include "acceleration/bvh.h"
#include <iostream>

int main(int argc, char* argv[]) {
    const std::string path = (argc > 1) ? argv[1] : "scenes/default.json";

    SceneConfig cfg = load_scene(path);

    Image image = cfg.renderer.render(*cfg.camera, BVHNode(cfg.world), cfg.width, cfg.height);
    image.save_ppm("output/render.ppm", cfg.renderer.samples_per_pixel);

    return 0;
}
