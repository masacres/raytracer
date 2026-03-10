#pragma once

#include "materials/material.h"
#include "textures/texture.h"
#include <memory>

class OpaqueMaterial : public Material {
public:
    std::shared_ptr<Texture> albedo;
    double specular = 0.0;
    double specular_pow = 0.0;
    double reflection = 0.0;

    explicit OpaqueMaterial(const Color& a, double spec = 0.0, double spec_pow = 32.0, double refl = 0.0);
    explicit OpaqueMaterial(std::shared_ptr<Texture> a, double spec = 0.0, double spec_pow = 32.0, double refl = 0.0)
        : albedo(std::move(a)), specular(spec), specular_pow(spec_pow), reflection(refl) {}

    bool scatter(const Ray& r_in, const HitRecord& rec,
                 Color& attenuation, Ray& scattered) const override;
    double get_specular() const override { return specular; }
    double get_specular_pow() const override { return specular_pow; }
    double get_reflection() const override { return reflection; }
};