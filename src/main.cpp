#include "core/camera.h"
#include "core/utils.h"
#include "geometry/hittable_list.h"
#include "geometry/sphere.h"
#include "materials/dielectric.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
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
        std::make_shared<Lambertian>(ground_tex)));

    // Three hero spheres
    world.add(std::make_shared<Sphere>(
        Point3(0, 1, 0), 1.0,
        std::make_shared<Dielectric>(1.5)));

    world.add(std::make_shared<Sphere>(
        Point3(-4, 1, 0), 1.0,
        std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1))));

    world.add(std::make_shared<Sphere>(
        Point3(4, 1, 0), 1.0,
        std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0)));

    // Random small spheres
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((center - Point3(4, 0.2, 0)).length() <= 0.9) continue;

            double mat_roll = random_double();
            if (mat_roll < 0.8) {
                world.add(std::make_shared<Sphere>(center, 0.2,
                    std::make_shared<Lambertian>(Color::random() * Color::random())));
            } else if (mat_roll < 0.95) {
                world.add(std::make_shared<Sphere>(center, 0.2,
                    std::make_shared<Metal>(Color::random(0.5, 1), random_double(0, 0.5))));
            } else {
                world.add(std::make_shared<Sphere>(center, 0.2,
                    std::make_shared<Dielectric>(1.5)));
            }
        }
    }
    return world;
}

int main() {
    // Image
    constexpr double aspect_ratio = 16.0 / 9.0;
    constexpr int    image_width  = 400;
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
    renderer.samples_per_pixel = 50;
    renderer.max_depth         = 50;

    Image image = renderer.render(cam, BVHNode(world), image_width, image_height);
    image.save_ppm("output/render.ppm", renderer.samples_per_pixel);

    return 0;
}
