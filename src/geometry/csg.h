#pragma once

#include "geometry/hittable.h"
#include <memory>
#include <string>

enum class CSGOperation { Union, Intersection, Difference };

class CSGNode : public Hittable {
public:
    CSGNode(CSGOperation op,
            std::shared_ptr<Hittable> left,
            std::shared_ptr<Hittable> right);

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;

private:
    CSGOperation              m_op;
    std::shared_ptr<Hittable> m_left;
    std::shared_ptr<Hittable> m_right;
};
