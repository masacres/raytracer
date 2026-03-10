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
    virtual double get_specular() const { return 0.0; }
    virtual double get_specular_pow() const { return 32.0; }
};
