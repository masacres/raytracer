#include "geometry/cuboid.h"
#include <algorithm>
#include <cmath>
#include <limits>

Cuboid::Cuboid(Point3 center, Vec3 u, Vec3 v,
               double width, double height, double depth,
               std::shared_ptr<Material> mat)
    : center(center)
    , half_w(width  / 2.0)
    , half_h(height / 2.0)
    , half_d(depth  / 2.0)
    , mat(std::move(mat))
{
    u_axis = unit_vector(u);
    w_axis = unit_vector(cross(u_axis, v));   // perpendicular to both
    v_axis = cross(w_axis, u_axis);           // recomputed to guarantee orthogonality
}

bool Cuboid::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    // Project ray into the cuboid's local frame.
    Vec3 oc = r.origin() - center;
    double ox = dot(oc, u_axis), oy = dot(oc, v_axis), oz = dot(oc, w_axis);
    double dx = dot(r.direction(), u_axis);
    double dy = dot(r.direction(), v_axis);
    double dz = dot(r.direction(), w_axis);

    const double inf = std::numeric_limits<double>::infinity();

    // Per-axis slab intervals and the axis index that produced t_near / t_far.
    double t_near = -inf, t_far = inf;
    int    near_axis = 0, far_axis = 0;

    auto process_axis = [&](double o, double d, double half, int axis) -> bool {
        if (std::abs(d) < 1e-10) {
            // Ray parallel to slab — check if origin is inside.
            if (std::abs(o) > half) return false;
        } else {
            double inv = 1.0 / d;
            double t1  = (-half - o) * inv;
            double t2  = ( half - o) * inv;
            if (t1 > t2) std::swap(t1, t2);

            if (t1 > t_near) { t_near = t1; near_axis = axis; }
            if (t2 < t_far)  { t_far  = t2; far_axis  = axis; }

            if (t_far < t_near) return false;
        }
        return true;
    };

    if (!process_axis(ox, dx, half_w, 0)) return false;
    if (!process_axis(oy, dy, half_h, 1)) return false;
    if (!process_axis(oz, dz, half_d, 2)) return false;

    // Pick the first t inside [t_min, t_max].
    double t;
    int    hit_axis;
    bool   entering;   // true → hit near face, false → hit far face (ray starts inside)
    if (t_near >= t_min && t_near <= t_max) {
        t = t_near; hit_axis = near_axis; entering = true;
    } else if (t_far >= t_min && t_far <= t_max) {
        t = t_far;  hit_axis = far_axis;  entering = false;
    } else {
        return false;
    }

    // Outward normal: for a near-face hit the ray travels in the +d direction,
    // so the outward normal points opposite to d.  For a far-face hit it is the
    // same side as d (ray exits through the face it faces).
    const Vec3 axes[3] = { u_axis, v_axis, w_axis };
    double      dirs[3] = { dx, dy, dz };
    Vec3 outward;
    if (entering)
        outward = (dirs[hit_axis] > 0.0) ? -axes[hit_axis] : axes[hit_axis];
    else
        outward = (dirs[hit_axis] > 0.0) ?  axes[hit_axis] : -axes[hit_axis];

    rec.t = t;
    rec.p = r.at(t);
    rec.set_face_normal(r, outward);
    rec.mat = mat;
    return true;
}

bool Cuboid::bounding_box(AABB& output_box) const {
    // Expand all 8 corners in world space and take the min/max envelope.
    Point3 mn( std::numeric_limits<double>::infinity(),
               std::numeric_limits<double>::infinity(),
               std::numeric_limits<double>::infinity());
    Point3 mx(-std::numeric_limits<double>::infinity(),
              -std::numeric_limits<double>::infinity(),
              -std::numeric_limits<double>::infinity());

    for (int i = -1; i <= 1; i += 2)
    for (int j = -1; j <= 1; j += 2)
    for (int k = -1; k <= 1; k += 2) {
        Point3 corner = center
            + (i * half_w) * u_axis
            + (j * half_h) * v_axis
            + (k * half_d) * w_axis;

        for (int a = 0; a < 3; a++) {
            if (corner[a] < mn[a]) mn[a] = corner[a];
            if (corner[a] > mx[a]) mx[a] = corner[a];
        }
    }

    output_box = AABB(mn, mx);
    return true;
}
