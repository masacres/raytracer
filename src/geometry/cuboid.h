#pragma once
#include "geometry/hittable.h"
#include <memory>

class Cuboid : public Hittable {
public:
    Point3                    center;
    Vec3                      u_axis;   // unit vector along width
    Vec3                      v_axis;   // unit vector along height
    Vec3                      w_axis;   // unit vector along depth  (= cross(u, v))
    double                    half_w;   // width  / 2
    double                    half_h;   // height / 2
    double                    half_d;   // depth  / 2
    std::shared_ptr<Material> mat;

    // u and v define the orientation; w is derived as cross(u, v).
    // u and v are orthogonalised internally — they do not need to be
    // perfectly perpendicular in the JSON, but they must not be parallel.
    Cuboid(Point3 center, Vec3 u, Vec3 v,
           double width, double height, double depth,
           std::shared_ptr<Material> mat);

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;
};
