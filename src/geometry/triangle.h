#pragma once
#include "geometry/hittable.h"
#include <memory>

class Triangle : public Hittable {
public:
    Point3                    p0, p1, p2;  // vertices
    Vec3                      normal;       // precomputed face normal (unit)
    std::shared_ptr<Material> mat;

    Triangle(Point3 p0, Point3 p1, Point3 p2, std::shared_ptr<Material> mat);

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;
};
