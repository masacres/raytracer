#pragma once
#include "geometry/hittable.h"
#include <memory>

class Cylinder : public Hittable {
public:
    Point3                    center;   // midpoint of the axis
    Vec3                      axis;     // unit direction vector
    Vec3                      x_axis;  // reference direction perpendicular to axis
    Vec3                      z_axis;  // completes the local frame
    double                    radius;
    double                    height;   // total height; caps at ±height/2 along axis
    std::shared_ptr<Material> mat;

    Cylinder(Point3 center, Vec3 axis, double radius, double height,
             std::shared_ptr<Material> mat);

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;
};
