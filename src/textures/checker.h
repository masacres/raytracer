#pragma once

#include "textures/texture.h"
#include <memory>

class CheckerTexture : public Texture {
public:
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
    double frequency = 10.0;

    CheckerTexture() = default;
    CheckerTexture(std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd, double freq)
        : even(std::move(even)), odd(std::move(odd)), frequency(freq) {}
    CheckerTexture(Color c1, Color c2, double freq);  // convenience: wraps in SolidColor

    Color value(double u, double v, const Point3& p) const override;
};
