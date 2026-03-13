#include "geometry/plane.h"
#include <cmath>

Plane::Plane(Point3 p, Vec3 n, std::shared_ptr<Material> m)
    : point(p), normal(unit_vector(n)), mat(std::move(m)) {
    // Build two orthogonal tangent vectors in the plane.
    Vec3 ref = (std::fabs(normal.x()) < 0.9) ? Vec3(1, 0, 0) : Vec3(0, 1, 0);
    u_axis = unit_vector(cross(normal, ref));
    v_axis = cross(normal, u_axis);
}

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
    Vec3 offset = rec.p - point;
    rec.u = dot(offset, u_axis);
    rec.v = dot(offset, v_axis);
    return true;
}

bool Plane::bounding_box(AABB& /*output_box*/) const {
    return false;  // infinite plane has no finite AABB
}
