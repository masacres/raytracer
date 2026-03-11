#include "textures/uv_checker.h"
#include "textures/solid_color.h"
#include <cmath>

UVCheckerTexture::UVCheckerTexture(Color c1, Color c2, double tiles_u, double tiles_v)
    : even(std::make_shared<SolidColor>(c1))
    , odd (std::make_shared<SolidColor>(c2))
    , tiles_u(tiles_u), tiles_v(tiles_v) {}

Color UVCheckerTexture::value(double u, double v, const Point3& p) const {
    int iu = static_cast<int>(std::floor(u * tiles_u));
    int iv = static_cast<int>(std::floor(v * tiles_v));
    bool is_even = ((iu + iv) & 1) == 0;
    return (is_even ? even : odd)->value(u, v, p);
}
