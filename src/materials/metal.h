#pragma once

#include "materials/material.h"

class Metal : public Material {
public:
    Color  albedo;
    double fuzz;

    Metal(const Color& a, double f) : albedo(a), fuzz(f < 1.0 ? f : 1.0) {}

    bool scatter(const Ray& r_in, const HitRecord& rec,
                 Color& attenuation, Ray& scattered) const override;
};
