#include "materials/opaque_material.h"
#include "textures/solid_color.h"

OpaqueMaterial::OpaqueMaterial(const Color& a, double spec, double spec_pow, double refl)
    : albedo(std::make_shared<SolidColor>(a)), specular(spec), specular_pow(spec_pow), reflection(refl) {}

bool OpaqueMaterial::scatter(const Ray& r_in, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const {
    attenuation = albedo->value(0, 0, rec.p);
    if (reflection > 0.0) {
        Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected);
        return true;
    }
    Vec3 dir = rec.normal + random_unit_vector();
    if (dir.near_zero()) dir = rec.normal;
    scattered = Ray(rec.p, dir);
    return false;
}
