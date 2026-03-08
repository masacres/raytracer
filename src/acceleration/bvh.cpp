#include "acceleration/bvh.h"
#include "core/utils.h"
#include <algorithm>
#include <stdexcept>

static bool box_compare(const std::shared_ptr<Hittable>& a,
                        const std::shared_ptr<Hittable>& b, int axis) {
    AABB ba, bb;
    if (!a->bounding_box(ba) || !b->bounding_box(bb))
        throw std::runtime_error("No bounding box in BVH construction");
    return ba.minimum[axis] < bb.minimum[axis];
}

BVHNode::BVHNode(const HittableList& list) {
    auto objects = list.objects;  // copy
    *this = BVHNode(objects, 0, objects.size());
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end) {
    int    axis = random_int(0, 2);
    size_t span = end - start;

    if (span == 1) {
        left = right = objects[start];
    } else if (span == 2) {
        bool ordered = box_compare(objects[start], objects[start + 1], axis);
        left  = objects[start + (ordered ? 0 : 1)];
        right = objects[start + (ordered ? 1 : 0)];
    } else {
        std::sort(objects.begin() + start, objects.begin() + end,
                  [axis](const auto& a, const auto& b) { return box_compare(a, b, axis); });
        size_t mid = start + span / 2;
        left  = std::make_shared<BVHNode>(objects, start, mid);
        right = std::make_shared<BVHNode>(objects, mid,   end);
    }

    AABB bl, br;
    if (!left->bounding_box(bl) || !right->bounding_box(br))
        throw std::runtime_error("No bounding box in BVH construction");
    box = surrounding_box(bl, br);
}

bool BVHNode::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    if (!box.hit(r, t_min, t_max)) return false;
    bool hit_left  = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
    return hit_left || hit_right;
}

bool BVHNode::bounding_box(AABB& output_box) const {
    output_box = box;
    return true;
}
