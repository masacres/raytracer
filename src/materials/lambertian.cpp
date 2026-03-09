#include "materials/lambertian.h"
#include "textures/solid_color.h"

Lambertian::Lambertian(const Color& a)
    : albedo(std::make_shared<SolidColor>(a)) {}

bool Lambertian::scatter(const Ray& /*r_in*/, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const {
    Vec3 dir = rec.normal;// + random_unit_vector();
    if (dir.near_zero()) dir = rec.normal;
    scattered   = Ray(rec.p, dir);
    attenuation = albedo->value(0, 0, rec.p);
    return false;
}
