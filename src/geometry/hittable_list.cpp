#include "geometry/hittable_list.h"

bool HittableList::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    HitRecord temp;
    bool hit_anything    = false;
    double closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp)) {
            hit_anything    = true;
            closest_so_far  = temp.t;
            rec             = temp;
        }
    }
    return hit_anything;
}

bool HittableList::bounding_box(AABB& output_box) const {
    if (objects.empty()) return false;

    AABB temp_box;
    bool first = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(temp_box)) return false;
        output_box = first ? temp_box : surrounding_box(output_box, temp_box);
        first = false;
    }
    return true;
}
