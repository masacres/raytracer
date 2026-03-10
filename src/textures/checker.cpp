#include "textures/checker.h"
#include "textures/solid_color.h"
#include <cmath>

CheckerTexture::CheckerTexture(Color c1, Color c2, double freq)
    : even(std::make_shared<SolidColor>(c1))
    , odd (std::make_shared<SolidColor>(c2)), frequency(freq) {}

Color CheckerTexture::value(double u, double v, const Point3& p) const {
    double sines = std::sin(frequency * p.x()) * std::sin(frequency * p.y()) * std::sin(frequency * p.z());
    return (sines < 0 ? odd : even)->value(u, v, p);
}
