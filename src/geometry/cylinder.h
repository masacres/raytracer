#pragma once
#include "geometry/hittable.h"
#include <memory>

class Cylinder : public Hittable {
public:
    Point3                    center;       // base center (h=0)
    Vec3                      axis;         // unit direction vector (base → top)
    Vec3                      x_axis;       // reference direction perpendicular to axis
    Vec3                      z_axis;       // completes the local frame
    double                    base_radius;  // radius at center (h=0)
    double                    top_radius;   // radius at center + height*axis (h=height)
    double                    height;       // distance from base to top cap
    std::shared_ptr<Material> mat;

    Cylinder(Point3 center, Vec3 axis, double base_radius, double top_radius,
             double height, std::shared_ptr<Material> mat);

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;
};
