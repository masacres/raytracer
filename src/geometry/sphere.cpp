#include "geometry/sphere.h"
#include <cmath>

bool Sphere::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    Vec3   oc   = r.origin() - center;
    double a    = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c    = oc.length_squared() - radius * radius;
    double disc = half_b * half_b - a * c;

    if (disc < 0) return false;
    double sqrtd = std::sqrt(disc);

    // Nearest root in [t_min, t_max]
    double root = (-half_b - sqrtd) / a;
    if (root < t_min || root > t_max) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || root > t_max)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    rec.set_face_normal(r, (rec.p - center) / radius);
    Vec3 n = (rec.p - center) / radius;
    double theta = std::acos(-n.y());
    double phi   = std::atan2(-n.z(), n.x()) + M_PI;
    rec.u = phi   / (2.0 * M_PI);
    rec.v = theta / M_PI;
    rec.mat = mat;
    return true;
}

bool Sphere::bounding_box(AABB& output_box) const {
    output_box = AABB(
        center - Vec3(radius, radius, radius),
        center + Vec3(radius, radius, radius)
    );
    return true;
}
