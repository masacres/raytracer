#include "scene/scene_loader.h"
#include "acceleration/bvh.h"
#include "geometry/hittable_list.h"
#include <iostream>

int main(int argc, char* argv[]) {
    const std::string path = (argc > 1) ? argv[1] : "scenes/default.json";

    SceneConfig cfg = load_scene(path);

    // Separate bounded and unbounded objects — unbounded ones (e.g. planes)
    // cannot go into the BVH.
    HittableList bounded, unbounded;
    for (const auto& obj : cfg.world.objects) {
        AABB box;
        if (obj->bounding_box(box))
            bounded.add(obj);
        else
            unbounded.add(obj);
    }

    // Build the scene: unbounded objects tested linearly, bounded ones via BVH.
    HittableList scene;
    for (const auto& obj : unbounded.objects)
        scene.add(obj);
    if (!bounded.objects.empty())
        scene.add(std::make_shared<BVHNode>(bounded));

    Image image = cfg.renderer.render(*cfg.camera, scene, cfg.width, cfg.height);
    image.save_ppm("output/render.ppm", cfg.renderer.samples_per_pixel);

    return 0;
}
