#pragma once

#include "geometry/hittable.h"
#include "geometry/hittable_list.h"
#include <memory>
#include <vector>

class BVHNode : public Hittable {
public:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;

    BVHNode() = default;
    explicit BVHNode(const HittableList& list);
    BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end);

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(AABB& output_box) const override;
};
