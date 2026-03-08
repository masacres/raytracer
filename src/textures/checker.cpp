#include "textures/checker.h"
#include "textures/solid_color.h"
#include <cmath>

CheckerTexture::CheckerTexture(Color c1, Color c2)
    : even(std::make_shared<SolidColor>(c1))
    , odd (std::make_shared<SolidColor>(c2)) {}

Color CheckerTexture::value(double u, double v, const Point3& p) const {
    double sines = std::sin(10 * p.x()) * std::sin(10 * p.y()) * std::sin(10 * p.z());
    return (sines < 0 ? odd : even)->value(u, v, p);
}
