#pragma once

#include "core/ray.h"
#include "core/vec3.h"
#include "geometry/hittable.h"  // for HitRecord

class Material {
public:
    virtual ~Material() = default;
    virtual bool scatter(
        const Ray& r_in, const HitRecord& rec,
        Color& attenuation, Ray& scattered
    ) const = 0;
};
