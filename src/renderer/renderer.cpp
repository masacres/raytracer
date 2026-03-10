#include "renderer/renderer.h"
#include "materials/material.h"
#include "core/utils.h"
#include <algorithm>
#include <iostream>

Color Renderer::ray_color(const Ray& r, const Hittable& world, int depth) const {
    if (depth <= 0) return sky_color;

    HitRecord rec;
    if (!world.hit(r, 0.001, infinity, rec))
        return sky_color;

    Ray   scattered;
    Color attenuation;
    rec.mat->scatter(r, rec, attenuation, scattered);  // sets attenuation only

    double trans = rec.mat->get_transparency();
    double refl  = rec.mat->get_reflection();

    // Ambient — attenuated by opacity
    Color result = (1.0 - trans) * ambient * attenuation;

    // Shadow ray toward directional light
    Vec3 to_light = -light_dir;
    Ray  shadow_ray(rec.p, to_light);
    HitRecord shadow_rec;
    if (!world.hit(shadow_ray, 0.001, infinity, shadow_rec)) {
        double ndotl = std::max(dot(rec.normal, to_light), 0.0);
        result += (1.0 - trans) * light_color * attenuation * ndotl;

        double spec = rec.mat->get_specular();
        if (spec > 0.0) {
            Vec3 reflect_dir = reflect(light_dir, rec.normal);
            Vec3 view_dir    = -unit_vector(r.direction());
            double rv        = std::max(dot(reflect_dir, view_dir), 0.0);
            result += (1.0 - trans) * spec * light_color * std::pow(rv, rec.mat->get_specular_pow());
        }
    }

    // Transmitted bounce
    if (trans > 0.0) {
        Ray transmitted(rec.p, r.direction());
        result += trans * attenuation * ray_color(transmitted, world, depth - 1);
    }

    // Reflected bounce
    if (refl > 0.0) {
        Vec3 ref_dir = reflect(unit_vector(r.direction()), rec.normal);
        result += refl * attenuation * ray_color(Ray(rec.p, ref_dir), world, depth - 1);
    }

    return result;
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
            /*for (int s = 0; s < samples_per_pixel; s++) {
                double u = (i + random_double()) / (width  - 1);
                double v = (j + random_double()) / (height - 1);
                pixel += ray_color(cam.get_ray(u, v), world, max_depth);
            }*/
            image.at(i, j) = pixel;
        }
    }
    std::cerr << "\nDone.\n";
    return image;
}
