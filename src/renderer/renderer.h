#pragma once

#include "core/camera.h"
#include "core/color.h"
#include "geometry/hittable.h"
#include "renderer/image.h"

class Renderer {
public:
    int samples_per_pixel = 100;
    int max_depth         = 50;

    Image render(const Camera& cam, const Hittable& world,
                 int width, int height) const;

private:
    Color ray_color(const Ray& r, const Hittable& world, int depth) const;
};
