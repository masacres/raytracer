#pragma once

#include "textures/texture.h"
#include <memory>

// Checker pattern tiled in UV space rather than world space.
// One full checker period spans u=[0,1], v=[0,1] on the surface.
// Use `tiles_u` and `tiles_v` to control how many checker squares appear.
class UVCheckerTexture : public Texture {
public:
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
    double tiles_u = 2.0;
    double tiles_v = 2.0;

    UVCheckerTexture(std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd,
                     double tiles_u, double tiles_v)
        : even(std::move(even)), odd(std::move(odd)), tiles_u(tiles_u), tiles_v(tiles_v) {}

    UVCheckerTexture(Color c1, Color c2, double tiles_u = 2.0, double tiles_v = 2.0);

    Color value(double u, double v, const Point3& p) const override;
};
