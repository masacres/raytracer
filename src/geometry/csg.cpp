#include "geometry/csg.h"
#include "geometry/aabb.h"
#include <vector>
#include <algorithm>

// Collect all intersections with obj in [t_min, t_max], advancing past each hit.
static std::vector<HitRecord> collect_hits(
        const Hittable& obj, const Ray& r, double t_min, double t_max) {
    std::vector<HitRecord> hits;
    double t = t_min;
    HitRecord rec;
    while (obj.hit(r, t + 1e-6, t_max, rec)) {
        hits.push_back(rec);
        t = rec.t;
    }
    return hits;
}

CSGNode::CSGNode(CSGOperation op,
                 std::shared_ptr<Hittable> left,
                 std::shared_ptr<Hittable> right)
    : m_op(op), m_left(std::move(left)), m_right(std::move(right)) {}

bool CSGNode::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    auto L_hits = collect_hits(*m_left,  r, t_min, t_max);
    auto R_hits = collect_hits(*m_right, r, t_min, t_max);

    // Determine initial inside/outside state before t_min.
    // If the first hit is an exit (front_face==false) we were already inside.
    bool inside_L = !L_hits.empty() && !L_hits[0].front_face;
    bool inside_R = !R_hits.empty() && !R_hits[0].front_face;

    // Merge both lists sorted by t, tagging each with its source.
    struct TaggedHit { double t; bool from_left; std::size_t idx; };
    std::vector<TaggedHit> merged;
    merged.reserve(L_hits.size() + R_hits.size());
    for (std::size_t i = 0; i < L_hits.size(); ++i)
        merged.push_back({L_hits[i].t, true,  i});
    for (std::size_t i = 0; i < R_hits.size(); ++i)
        merged.push_back({R_hits[i].t, false, i});
    std::sort(merged.begin(), merged.end(),
              [](const TaggedHit& a, const TaggedHit& b){ return a.t < b.t; });

    auto in_csg = [&]() -> bool {
        switch (m_op) {
            case CSGOperation::Union:        return inside_L || inside_R;
            case CSGOperation::Intersection: return inside_L && inside_R;
            case CSGOperation::Difference:   return inside_L && !inside_R;
        }
        return false;
    };

    for (const auto& th : merged) {
        bool was_in = in_csg();

        // Toggle the relevant side.
        if (th.from_left)  inside_L = !inside_L;
        else               inside_R = !inside_R;

        bool now_in = in_csg();

        // A false→true transition is the CSG surface entry.
        if (!was_in && now_in) {
            rec = th.from_left ? L_hits[th.idx] : R_hits[th.idx];
            return true;
        }
    }

    return false;
}

bool CSGNode::bounding_box(AABB& output_box) const {
    AABB left_box, right_box;
    bool has_left  = m_left->bounding_box(left_box);
    bool has_right = m_right->bounding_box(right_box);

    switch (m_op) {
        case CSGOperation::Union:
            if (!has_left || !has_right) return false;
            output_box = surrounding_box(left_box, right_box);
            return true;

        case CSGOperation::Intersection:
            // Conservative: use surrounding box (can't be smaller than the intersection).
            if (!has_left || !has_right) return false;
            output_box = surrounding_box(left_box, right_box);
            return true;

        case CSGOperation::Difference:
            // A−B ⊆ A.
            if (!has_left) return false;
            output_box = left_box;
            return true;
    }
    return false;
}
