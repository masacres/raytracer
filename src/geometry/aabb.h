#pragma once

#include "core/ray.h"
#include "core/vec3.h"
#include <algorithm>

class AABB {
public:
    Point3 minimum;
    Point3 maximum;

    AABB() = default;
    AABB(const Point3& a, const Point3& b) : minimum(a), maximum(b) {}

    bool hit(const Ray& r, double t_min, double t_max) const {
        for (int a = 0; a < 3; a++) {
            double inv_d = 1.0 / r.direction()[a];
            double t0 = (minimum[a] - r.origin()[a]) * inv_d;
            double t1 = (maximum[a] - r.origin()[a]) * inv_d;
            if (inv_d < 0) std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min) return false;
        }
        return true;
    }
};

inline AABB surrounding_box(const AABB& box0, const AABB& box1) {
    Point3 small(
        std::fmin(box0.minimum.x(), box1.minimum.x()),
        std::fmin(box0.minimum.y(), box1.minimum.y()),
        std::fmin(box0.minimum.z(), box1.minimum.z())
    );
    Point3 big(
        std::fmax(box0.maximum.x(), box1.maximum.x()),
        std::fmax(box0.maximum.y(), box1.maximum.y()),
        std::fmax(box0.maximum.z(), box1.maximum.z())
    );
    return AABB(small, big);
}
