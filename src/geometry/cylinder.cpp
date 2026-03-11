#include "geometry/cylinder.h"
#include <cmath>
#include <limits>

Cylinder::Cylinder(Point3 center, Vec3 axis, double radius, double height,
                   std::shared_ptr<Material> mat)
    : center(center), axis(unit_vector(axis)), radius(radius), height(height),
      mat(std::move(mat)) {}

bool Cylinder::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    const double half_h = height / 2.0;
    const Vec3   orig   = r.origin() - center;
    const Vec3   dir    = r.direction();

    double best_t = std::numeric_limits<double>::infinity();
    Vec3   best_normal;

    // ---- Side surface ----
    Vec3   e_perp = orig - dot(orig, axis) * axis;
    Vec3   v_perp = dir  - dot(dir,  axis) * axis;

    double a      = v_perp.length_squared();
    if (a > 1e-12) {
        double half_b = dot(e_perp, v_perp);
        double c      = e_perp.length_squared() - radius * radius;
        double disc   = half_b * half_b - a * c;

        if (disc >= 0.0) {
            double sqrtd = std::sqrt(disc);
            double roots[2] = { (-half_b - sqrtd) / a, (-half_b + sqrtd) / a };
            for (double t : roots) {
                if (t < t_min || t > t_max) continue;
                double h_val = dot(r.at(t) - center, axis);
                if (std::abs(h_val) > half_h) continue;
                if (t < best_t) {
                    best_t      = t;
                    best_normal = (r.at(t) - center - h_val * axis) / radius;
                }
            }
        }
    }

    // ---- Caps ----
    double denom = dot(dir, axis);
    if (std::abs(denom) > 1e-12) {
        // Top cap
        {
            Point3 cap_center = center + half_h * axis;
            double t = dot(cap_center - r.origin(), axis) / denom;
            if (t >= t_min && t <= t_max && t < best_t) {
                Point3 p = r.at(t);
                if ((p - cap_center).length_squared() <= radius * radius) {
                    best_t      = t;
                    best_normal = axis;
                }
            }
        }
        // Bottom cap
        {
            Point3 cap_center = center - half_h * axis;
            double t = dot(cap_center - r.origin(), axis) / denom;
            if (t >= t_min && t <= t_max && t < best_t) {
                Point3 p = r.at(t);
                if ((p - cap_center).length_squared() <= radius * radius) {
                    best_t      = t;
                    best_normal = -axis;
                }
            }
        }
    }

    if (best_t == std::numeric_limits<double>::infinity()) return false;

    rec.t = best_t;
    rec.p = r.at(rec.t);
    rec.set_face_normal(r, best_normal);
    rec.mat = mat;
    return true;
}

bool Cylinder::bounding_box(AABB& output_box) const {
    double half_h = height / 2.0;
    double ax = axis.x(), ay = axis.y(), az = axis.z();
    double half_x = radius * std::sqrt(std::max(0.0, 1.0 - ax*ax)) + std::abs(ax) * half_h;
    double half_y = radius * std::sqrt(std::max(0.0, 1.0 - ay*ay)) + std::abs(ay) * half_h;
    double half_z = radius * std::sqrt(std::max(0.0, 1.0 - az*az)) + std::abs(az) * half_h;
    output_box = AABB(center - Vec3(half_x, half_y, half_z),
                      center + Vec3(half_x, half_y, half_z));
    return true;
}
