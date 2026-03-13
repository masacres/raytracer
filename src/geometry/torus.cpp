#include "geometry/torus.h"
#include <cmath>
#include <algorithm>
#include <vector>

// ────────────────────────── Robust Quartic Solver ──────────────────────────

static std::vector<double> solve_quartic_robust(double a, double b, double c, double d, double e) {
    std::vector<double> roots;
    if (std::fabs(a) < 1e-12) return roots;

    // Normalize
    double a3 = b / a, a2 = c / a, a1 = d / a, a0 = e / a;

    // Resolvent cubic: y^3 - a2y^2 + (a1a3 - 4a0)y + (4a2a0 - a1^2 - a3^2a0) = 0
    double cb_a2 = -a2;
    double cb_a1 = a1 * a3 - 4.0 * a0;
    double cb_a0 = 4.0 * a2 * a0 - a1 * a1 - a3 * a3 * a0;

    // Standard Cubic Solver
    auto solve_cubic_local = [](double c2, double c1, double c0) -> double {
        double Q = (c2 * c2 - 3.0 * c1) / 9.0;
        double R = (2.0 * c2 * c2 * c2 - 9.0 * c2 * c1 + 27.0 * c0) / 54.0;
        if (R * R < Q * Q * Q) {
            double theta = std::acos(R / std::sqrt(Q * Q * Q));
            return -2.0 * std::sqrt(Q) * std::cos(theta / 3.0) - c2 / 3.0;
        }
        return std::cbrt(-R + std::sqrt(R * R - Q * Q * Q)) + 
               std::cbrt(-R - std::sqrt(R * R - Q * Q * Q)) - c2 / 3.0;
    };

    double y = solve_cubic_local(cb_a2, cb_a1, cb_a0);
    double D_sq = a3 * a3 / 4.0 - a2 + y;
    
    if (D_sq < 0) return roots;
    double D = std::sqrt(D_sq);
    double E_sq, F_sq;

    if (std::fabs(D) < 1e-12) {
        E_sq = y * y - 4.0 * a0;
        if (E_sq < 0) return roots;
        F_sq = std::sqrt(E_sq);
    } else {
        F_sq = (0.75 * a3 * a3 - D_sq - 2.0 * a2 + (4.0 * a3 * a2 - 8.0 * a1 - a3 * a3 * a3) / (4.0 * D)) / 1.0;
        E_sq = (0.75 * a3 * a3 - D_sq - 2.0 * a2 - (4.0 * a3 * a2 - 8.0 * a1 - a3 * a3 * a3) / (4.0 * D)) / 1.0;
    }

    auto solve_quad = [&](double b_q, double disc_q) {
        if (disc_q >= 0) {
            roots.push_back(-a3 / 4.0 + b_q + std::sqrt(disc_q) / 2.0);
            roots.push_back(-a3 / 4.0 + b_q - std::sqrt(disc_q) / 2.0);
        }
    };

    solve_quad(D / 2.0, F_sq);
    solve_quad(-D / 2.0, E_sq);

    return roots;
}

// ─────────────────────────── Toroid Implementation ──────────────────────────

Torus::Torus(Point3 cen, Vec3 ax, double R_in, double r_in, std::shared_ptr<Material> m)
    : center(cen), axis(unit_vector(ax)), R(R_in), r(r_in), mat(m) 
{
    Vec3 up = (std::fabs(axis.y()) < 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
    x_axis = unit_vector(cross(up, axis));
    z_axis = cross(x_axis, axis);
}

bool Torus::hit(const Ray& ray, double t_min, double t_max, HitRecord& rec) const {
    Vec3 oc = ray.origin() - center;
    Vec3 lo(dot(oc, x_axis), dot(oc, axis), dot(oc, z_axis));
    Vec3 ld(dot(ray.direction(), x_axis), dot(ray.direction(), axis), dot(ray.direction(), z_axis));

    double R2 = R * R, r2 = r * r;
    double ld2 = dot(ld, ld), lo2 = dot(lo, lo), lold = dot(lo, ld);

    double K = lo2 + R2 - r2;
    double a4 = ld2 * ld2;
    double a3 = 4.0 * ld2 * lold;
    double a2 = 2.0 * ld2 * K + 4.0 * lold * lold - 4.0 * R2 * (ld.x() * ld.x() + ld.z() * ld.z());
    double a1 = 4.0 * lold * K - 8.0 * R2 * (lo.x() * ld.x() + lo.z() * ld.z());
    double a0 = K * K - 4.0 * R2 * (lo.x() * lo.x() + lo.z() * lo.z());

    auto roots = solve_quartic_robust(a4, a3, a2, a1, a0);

    double t_best = t_max;
    bool found = false;

    for (double t : roots) {
        if (t > t_min && t < t_best) {
            // Newton-Raphson Polish
            double rt = t;
            for (int i = 0; i < 2; i++) {
                double f = a4*std::pow(rt,4) + a3*std::pow(rt,3) + a2*rt*rt + a1*rt + a0;
                double df = 4*a4*std::pow(rt,3) + 3*a3*rt*rt + 2*a2*rt + a1;
                rt -= f / df;
            }
            if (rt > t_min && rt < t_best) {
                t_best = rt;
                found = true;
            }
        }
    }

    if (!found) return false;

    rec.t = t_best;
    rec.p = ray.at(t_best);
    
    // Normal calculation
    Vec3 lp = lo + t_best * ld;
    double xz_len = std::sqrt(lp.x()*lp.x() + lp.z()*lp.z());
    Vec3 local_n = (xz_len > 1e-10) 
        ? Vec3(lp.x()*(1-R/xz_len), lp.y(), lp.z()*(1-R/xz_len))
        : Vec3(0, lp.y() > 0 ? 1 : -1, 0);

    rec.set_face_normal(ray, unit_vector(local_n.x()*x_axis + local_n.y()*axis + local_n.z()*z_axis));
    rec.mat = mat;
    return true;
}

bool Torus::bounding_box(AABB& output_box) const {
    double rad = R + r;
    output_box = AABB(center - Vec3(rad, rad, rad), center + Vec3(rad, rad, rad));
    return true;
}