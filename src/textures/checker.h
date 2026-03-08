#pragma once

#include "textures/texture.h"
#include <memory>

class CheckerTexture : public Texture {
public:
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;

    CheckerTexture() = default;
    CheckerTexture(std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
        : even(std::move(even)), odd(std::move(odd)) {}
    CheckerTexture(Color c1, Color c2);  // convenience: wraps in SolidColor

    Color value(double u, double v, const Point3& p) const override;
};
