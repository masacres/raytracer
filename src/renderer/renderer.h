#pragma once

#include "core/camera.h"
#include "core/color.h"
#include "core/vec3.h"
#include "geometry/hittable.h"
#include "renderer/image.h"

class Renderer {
public:
    int   samples_per_pixel = 100;
    int   max_depth         = 50;
    Vec3  light_dir         = unit_vector(Vec3(-1.0, -2.0, -1.0));
    Color light_color       = Color(1.0, 1.0, 1.0);
    Color ambient           = Color(0.05, 0.05, 0.05);
    Color sky_color         = Color(0.5, 0.5, 0.8);

    Image render(const Camera& cam, const Hittable& world,
                 int width, int height) const;

private:
    Color ray_color(const Ray& r, const Hittable& world, int depth) const;
};
