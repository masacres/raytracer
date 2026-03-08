#pragma once

#include "geometry/hittable.h"
#include <memory>

class Sphere : public Hittable {
public:
    Point3 center;
    double radius = 0.0;
    std::shared_ptr<Material> mat;

    Sphere() = default;
    Sphere(Point3 center, double radius, std::shared_ptr<Material> mat)
        : center(center), radius(radius), mat(std::move(mat)) {}

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;
};
