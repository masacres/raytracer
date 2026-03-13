#include "textures/checker.h"
#include "textures/solid_color.h"
#include <cmath>

XYZCheckerTexture::XYZCheckerTexture(Color c1, Color c2, double freq)
    : even(std::make_shared<SolidColor>(c1))
    , odd (std::make_shared<SolidColor>(c2)), frequency(freq) {}

Color XYZCheckerTexture::value(double u, double v, const Point3& p) const {
    int ix = static_cast<int>(std::floor(frequency * p.x()));
    int iy = static_cast<int>(std::floor(frequency * p.y()));
    int iz = static_cast<int>(std::floor(frequency * p.z()));
    bool is_even = ((ix + iy + iz) & 1) == 0;
    return (is_even ? even : odd)->value(u, v, p);
}
