#include "renderer/renderer.h"
#include "materials/material.h"
#include "core/utils.h"
#include <iostream>

Color Renderer::ray_color(const Ray& r, const Hittable& world, int depth) const {
    if (depth <= 0) return Color(0, 0, 0);

    HitRecord rec;
    if (world.hit(r, 0.001, infinity, rec)) {
        Ray   scattered;
        Color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return Color(0, 0, 0);
    }

    // Sky gradient
    Vec3   unit_dir = unit_vector(r.direction());
    double t        = 0.5 * (unit_dir.y() + 1.0);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

Image Renderer::render(const Camera& cam, const Hittable& world,
                       int width, int height) const {
    Image image(width, height);

    for (int j = height - 1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << "  " << std::flush;
        for (int i = 0; i < width; i++) {
            Color pixel(0, 0, 0);
            constexpr double offsets[4][2] = {
                {0.25, 0.25}, {0.75, 0.25},
                {0.25, 0.75}, {0.75, 0.75}
            };
            for (auto& o : offsets) {
                double u = (i + o[0]) / (width  - 1);
                double v = (j + o[1]) / (height - 1);
                pixel += ray_color(cam.get_ray(u, v), world, max_depth);
            }
//            for (int s = 0; s < samples_per_pixel; s++) {
//                double u = (i + random_double()) / (width  - 1);
//                double v = (j + random_double()) / (height - 1);
//                pixel += ray_color(cam.get_ray(u, v), world, max_depth);
//            }
            image.at(i, j) = pixel;
        }
    }
    std::cerr << "\nDone.\n";
    return image;
}
