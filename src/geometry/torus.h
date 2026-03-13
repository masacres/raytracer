#pragma once
#include "geometry/hittable.h"
#include <memory>

class Torus : public Hittable {
public:
    Point3                    center;
    Vec3                      axis;   // unit vector — axis of revolution
    double                    R;      // major radius (center to tube center)
    double                    r;      // minor radius (tube radius)
    std::shared_ptr<Material> mat;

    Torus(Point3 center, Vec3 axis, double R, double r,
           std::shared_ptr<Material> mat);

    bool hit(const Ray& ray, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;

private:
    // Orthonormal basis built from axis; stored to avoid per-ray recomputation.
    Vec3 x_axis, z_axis;  // axis == y_axis
};
