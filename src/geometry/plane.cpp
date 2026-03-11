#include "geometry/plane.h"
#include <cmath>

Plane::Plane(Point3 p, Vec3 n, std::shared_ptr<Material> m)
    : point(p), normal(unit_vector(n)), mat(std::move(m)) {}

bool Plane::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    double denom = dot(normal, r.direction());
    if (std::fabs(denom) < 1e-8)
        return false;

    double t = dot(normal, point - r.origin()) / denom;
    if (t < t_min || t > t_max)
        return false;

    rec.t   = t;
    rec.p   = r.at(t);
    rec.mat = mat;
    rec.set_face_normal(r, normal);
    return true;
}

bool Plane::bounding_box(AABB& /*output_box*/) const {
    return false;  // infinite plane has no finite AABB
}
