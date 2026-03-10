#include "materials/opaque_material.h"
#include "textures/solid_color.h"

OpaqueMaterial::OpaqueMaterial(const Color& a, double spec, double spec_pow, double refl, double trans)
    : albedo(std::make_shared<SolidColor>(a)), specular(spec), specular_pow(spec_pow), reflection(refl), transparency(trans) {}

bool OpaqueMaterial::scatter(const Ray& /*r_in*/, const HitRecord& rec,
                         Color& attenuation, Ray& /*scattered*/) const {
    attenuation = albedo->value(0, 0, rec.p);
    return false;
}
