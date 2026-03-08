#pragma once

#include "materials/material.h"
#include "textures/texture.h"
#include <memory>

class Lambertian : public Material {
public:
    std::shared_ptr<Texture> albedo;

    explicit Lambertian(const Color& a);
    explicit Lambertian(std::shared_ptr<Texture> a) : albedo(std::move(a)) {}

    bool scatter(const Ray& r_in, const HitRecord& rec,
                 Color& attenuation, Ray& scattered) const override;
};
