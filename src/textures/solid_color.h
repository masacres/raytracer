#pragma once

#include "textures/texture.h"

class SolidColor : public Texture {
public:
    Color color_value;

    SolidColor() = default;
    explicit SolidColor(Color c) : color_value(c) {}
    SolidColor(double r, double g, double b) : color_value(r, g, b) {}

    Color value(double u, double v, const Point3& p) const override;
};
