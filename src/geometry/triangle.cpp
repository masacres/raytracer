#include "geometry/triangle.h"
#include <algorithm>
#include <cmath>

Triangle::Triangle(Point3 p0, Point3 p1, Point3 p2, std::shared_ptr<Material> mat)
    : p0(p0), p1(p1), p2(p2), mat(std::move(mat))
{
    normal = unit_vector(cross(p1 - p0, p2 - p0));
}

bool Triangle::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    Vec3 edge1 = p1 - p0;
    Vec3 edge2 = p2 - p0;
    Vec3 h     = cross(r.direction(), edge2);
    double det = dot(edge1, h);

    if (std::abs(det) < 1e-8)
        return false;

    double inv_det = 1.0 / det;
    Vec3   s       = r.origin() - p0;
    double u       = inv_det * dot(s, h);

    if (u < 0.0 || u > 1.0)
        return false;

    Vec3   q = cross(s, edge1);
    double v = inv_det * dot(r.direction(), q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    double t = inv_det * dot(edge2, q);

    if (t < t_min || t > t_max)
        return false;

    rec.t = t;
    rec.u = u;
    rec.v = v;
    rec.p = r.at(t);
    rec.set_face_normal(r, normal);
    rec.mat = mat;
    return true;
}

bool Triangle::bounding_box(AABB& output_box) const {
    Point3 mn(
        std::min({p0.x(), p1.x(), p2.x()}),
        std::min({p0.y(), p1.y(), p2.y()}),
        std::min({p0.z(), p1.z(), p2.z()})
    );
    Point3 mx(
        std::max({p0.x(), p1.x(), p2.x()}),
        std::max({p0.y(), p1.y(), p2.y()}),
        std::max({p0.z(), p1.z(), p2.z()})
    );
    output_box = AABB(mn - Vec3(1e-4, 1e-4, 1e-4), mx + Vec3(1e-4, 1e-4, 1e-4));
    return true;
}
