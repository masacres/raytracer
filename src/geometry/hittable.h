#pragma once

#include "core/ray.h"
#include "core/vec3.h"
#include "geometry/aabb.h"
#include <memory>

class Material;  // forward declaration to break the hittable <-> material cycle

struct HitRecord {
    Point3 p;
    Vec3   normal;
    std::shared_ptr<Material> mat;
    double t          = 0.0;
    double u          = 0.0;
    double v          = 0.0;
    bool   front_face = false;

    // Sets normal to always point against the incoming ray.
    void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal     = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const = 0;
    virtual bool bounding_box(AABB& output_box) const = 0;
};
