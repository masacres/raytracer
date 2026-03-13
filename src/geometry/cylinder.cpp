#include "geometry/cylinder.h"
#include <cmath>
#include <limits>

Cylinder::Cylinder(Point3 center, Vec3 axis, double base_radius, double top_radius,
                   double height, std::shared_ptr<Material> mat)
    : center(center), axis(unit_vector(axis)),
      base_radius(base_radius), top_radius(top_radius), height(height),
      mat(std::move(mat))
{
    Vec3 up = (std::fabs(this->axis.y()) < 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
    x_axis = unit_vector(cross(up, this->axis));
    z_axis = cross(this->axis, x_axis);
}

bool Cylinder::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    const Vec3 e   = r.origin() - center;
    const Vec3 dir = r.direction();

    const double e_h   = dot(e,   axis);
    const double v_h   = dot(dir, axis);
    const Vec3   e_perp = e   - e_h * axis;
    const Vec3   v_perp = dir - v_h * axis;

    // slope of radius along axis
    const double k  = (top_radius - base_radius) / height;
    const double K  = k * v_h;
    const double R0 = base_radius + k * e_h;  // radius at ray-origin's projected h

    double best_t = std::numeric_limits<double>::infinity();
    Vec3   best_normal;
    int    hit_part = -1;  // 0=side, 1=top cap, 2=base cap

    // ---- Lateral surface ----
    // (|v_perp|² - K²) t² + 2(e_perp·v_perp - R0·K) t + (|e_perp|² - R0²) = 0
    double a = v_perp.length_squared() - K * K;
    if (std::fabs(a) > 1e-12) {
        double half_b = dot(e_perp, v_perp) - R0 * K;
        double c      = e_perp.length_squared() - R0 * R0;
        double disc   = half_b * half_b - a * c;

        if (disc >= 0.0) {
            double sqrtd = std::sqrt(disc);
            double roots[2] = { (-half_b - sqrtd) / a, (-half_b + sqrtd) / a };
            for (double t : roots) {
                if (t < t_min || t > t_max) continue;
                double h_val = e_h + t * v_h;
                if (h_val < 0.0 || h_val > height) continue;
                if (t < best_t) {
                    best_t   = t;
                    hit_part = 0;
                    // Normal: radial component minus k*r(h)*axis, then normalize
                    Vec3 perp_at_t = e_perp + t * v_perp;
                    double r_h = base_radius + k * h_val;
                    best_normal = unit_vector(perp_at_t - k * r_h * axis);
                }
            }
        }
    }

    // ---- Caps ----
    double denom = v_h;  // dot(dir, axis)
    if (std::fabs(denom) > 1e-12) {
        // Top cap (only if top_radius > 0)
        if (top_radius > 0.0) {
            Point3 cap_center = center + height * axis;
            double t = dot(cap_center - r.origin(), axis) / denom;
            if (t >= t_min && t <= t_max && t < best_t) {
                Point3 p = r.at(t);
                if ((p - cap_center).length_squared() <= top_radius * top_radius) {
                    best_t      = t;
                    best_normal = axis;
                    hit_part    = 1;
                }
            }
        }
        // Base cap
        if (base_radius > 0.0) {
            double t = dot(center - r.origin(), axis) / denom;
            if (t >= t_min && t <= t_max && t < best_t) {
                Point3 p = r.at(t);
                if ((p - center).length_squared() <= base_radius * base_radius) {
                    best_t      = t;
                    best_normal = -axis;
                    hit_part    = 2;
                }
            }
        }
    }

    if (best_t == std::numeric_limits<double>::infinity()) return false;

    rec.t = best_t;
    rec.p = r.at(rec.t);
    rec.set_face_normal(r, best_normal);

    // UV coordinates
    Vec3 local = rec.p - center;
    if (hit_part == 0) {
        double phi   = std::atan2(dot(local, z_axis), dot(local, x_axis));
        double h_val = dot(local, axis);
        rec.u = (phi + M_PI) / (2.0 * M_PI);
        rec.v = h_val / height;
    } else {
        // Cap: planar UV centered on cap, normalized by respective radius
        double cap_r = (hit_part == 1) ? top_radius : base_radius;
        Vec3 radial  = local - dot(local, axis) * axis;
        rec.u = (dot(radial, x_axis) / cap_r + 1.0) / 2.0;
        rec.v = (dot(radial, z_axis) / cap_r + 1.0) / 2.0;
    }

    rec.mat = mat;
    return true;
}

bool Cylinder::bounding_box(AABB& output_box) const {
    Vec3 top_center = center + height * axis;
    double min_x, min_y, min_z, max_x, max_y, max_z;

    // For each axis component, lateral extremes occur at h=0 (base) or h=height (top)
    double ax = axis.x(), ay = axis.y(), az = axis.z();

    double ext_base_x = base_radius * std::sqrt(std::max(0.0, 1.0 - ax*ax));
    double ext_top_x  = top_radius  * std::sqrt(std::max(0.0, 1.0 - ax*ax));
    min_x = std::min(center.x() - ext_base_x, top_center.x() - ext_top_x);
    max_x = std::max(center.x() + ext_base_x, top_center.x() + ext_top_x);

    double ext_base_y = base_radius * std::sqrt(std::max(0.0, 1.0 - ay*ay));
    double ext_top_y  = top_radius  * std::sqrt(std::max(0.0, 1.0 - ay*ay));
    min_y = std::min(center.y() - ext_base_y, top_center.y() - ext_top_y);
    max_y = std::max(center.y() + ext_base_y, top_center.y() + ext_top_y);

    double ext_base_z = base_radius * std::sqrt(std::max(0.0, 1.0 - az*az));
    double ext_top_z  = top_radius  * std::sqrt(std::max(0.0, 1.0 - az*az));
    min_z = std::min(center.z() - ext_base_z, top_center.z() - ext_top_z);
    max_z = std::max(center.z() + ext_base_z, top_center.z() + ext_top_z);

    output_box = AABB(Point3(min_x, min_y, min_z), Point3(max_x, max_y, max_z));
    return true;
}
