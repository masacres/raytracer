#pragma once
#include "geometry/hittable.h"
#include <memory>

class Plane : public Hittable {
public:
    Point3                    point;   // a point on the plane
    Vec3                      normal;  // unit normal
    std::shared_ptr<Material> mat;

    Plane(Point3 point, Vec3 normal, std::shared_ptr<Material> mat);

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;
};
