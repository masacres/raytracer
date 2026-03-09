#include "materials/dielectric.h"
#include <cmath>

double Dielectric::reflectance(double cosine, double ref_idx) {
    // Schlick approximation
    double r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow(1 - cosine, 5);
}

bool Dielectric::scatter(const Ray& r_in, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const {
    attenuation = Color(1.0, 1.0, 1.0);
    double ratio = rec.front_face ? (1.0 / ir) : ir;

    Vec3   unit_dir  = unit_vector(r_in.direction());
    double cos_theta = std::fmin(dot(-unit_dir, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    Vec3 dir = (ratio * sin_theta > 1.0 || reflectance(cos_theta, ratio) > 0.5)
               ? reflect(unit_dir, rec.normal)
               : refract(unit_dir, rec.normal, ratio);

    scattered = Ray(rec.p, dir);
    return true;
}
