#include "core/camera.h"
#include "core/utils.h"
#include "geometry/hittable_list.h"
#include "geometry/sphere.h"
#include "materials/opaque_material.h"
#include "textures/checker.h"
#include "acceleration/bvh.h"
#include "renderer/renderer.h"
#include <memory>

static HittableList build_scene() {
    HittableList world;

    // Checker ground
    auto ground_tex = std::make_shared<CheckerTexture>(
        Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    world.add(std::make_shared<Sphere>(
        Point3(0, -1000, 0), 1000.0,
        std::make_shared<OpaqueMaterial>(ground_tex, 0.4)));

    // Three hero spheres
    world.add(std::make_shared<Sphere>(
        Point3(-4, 1, 2), 1.0,
        std::make_shared<OpaqueMaterial>(Color(0.4, 0.2, 0.1))));

    world.add(std::make_shared<Sphere>(
        Point3(0, 1, 1), 1.0,
        std::make_shared<OpaqueMaterial>(Color(0.5, 0.6, 0.6), 0.8, 16.0)));

    world.add(std::make_shared<Sphere>(
        Point3(4, 1, 0), 1.0,
        std::make_shared<OpaqueMaterial>(Color(0.9, 0.3, 0.2), 0.9, 8.0)));

    // Random small spheres
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            Point3 center(a + 0.9 * random_double(), 0.15, b + 0.9 * random_double());
            if ((center - Point3(4, 0.15, 0)).length() <= 0.9) continue;

//            double mat_roll = random_double();
//            if (mat_roll < 0.8) {
                world.add(std::make_shared<Sphere>(center, 0.15,
                    std::make_shared<OpaqueMaterial>(Color::random(), random_double())));
//            }
        }
    }
    return world;
}

int main() {
    // Image
    constexpr double aspect_ratio = 16.0 / 9.0;
    constexpr int    image_width  = 800;
    constexpr int    image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera
    Camera cam(
        Point3(13, 2, 3),  // lookfrom
        Point3( 0, 0, 0),  // lookat
        Vec3  ( 0, 1, 0),  // vup
        20.0,              // vfov
        aspect_ratio,
        0.1,               // aperture
        10.0               // focus distance
    );

    // Scene
    HittableList world = build_scene();

    // Render
    Renderer renderer;
    renderer.samples_per_pixel = 4;
    renderer.max_depth         = 10;
    renderer.light_dir         = unit_vector(Vec3(-1.0, -2.0, -1.0));
    renderer.light_color       = Color(0.9, 0.9, 0.9);
    renderer.ambient           = Color(0.05, 0.05, 0.05);
    renderer.sky_color         = Color(0.5, 0.5, 0.8);

    Image image = renderer.render(cam, BVHNode(world), image_width, image_height);
    image.save_ppm("output/render.ppm", renderer.samples_per_pixel);

    return 0;
}
