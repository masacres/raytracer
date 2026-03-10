#pragma once

#include "materials/material.h"
#include "textures/texture.h"
#include <memory>

class OpaqueMaterial : public Material {
public:
    std::shared_ptr<Texture> albedo;

    explicit OpaqueMaterial(const Color& a);
    explicit OpaqueMaterial(std::shared_ptr<Texture> a) : albedo(std::move(a)) {}

    bool scatter(const Ray& r_in, const HitRecord& rec,
                 Color& attenuation, Ray& scattered) const override;
};