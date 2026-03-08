#pragma once

#include "materials/material.h"

class Dielectric : public Material {
public:
    double ir;  // index of refraction

    explicit Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const Ray& r_in, const HitRecord& rec,
                 Color& attenuation, Ray& scattered) const override;

private:
    static double reflectance(double cosine, double ref_idx);
};
