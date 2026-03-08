#include "textures/solid_color.h"

Color SolidColor::value(double /*u*/, double /*v*/, const Point3& /*p*/) const {
    return color_value;
}
