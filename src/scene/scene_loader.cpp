#include "scene/scene_loader.h"
#include "core/utils.h"
#include "geometry/sphere.h"
#include "materials/opaque_material.h"
#include "textures/checker.h"
#include "textures/solid_color.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

static Color parse_color(const json& j) {
    return Color(j[0].get<double>(), j[1].get<double>(), j[2].get<double>());
}

static std::shared_ptr<Texture> parse_texture(const json& j) {
    std::string type = j.at("type").get<std::string>();
    if (type == "solid") {
        return std::make_shared<SolidColor>(parse_color(j.at("color")));
    }
    if (type == "checker") {
        Color c1   = parse_color(j.at("color1"));
        Color c2   = parse_color(j.at("color2"));
        double freq = j.value("frequency", 10.0);
        return std::make_shared<CheckerTexture>(c1, c2, freq);
    }
    throw std::runtime_error("Unknown texture type: " + type);
}

static std::shared_ptr<Material> parse_material(const json& j) {
    std::string type = j.at("type").get<std::string>();
    if (type == "opaque") {
        std::shared_ptr<Texture> tex;
        if (j.contains("texture")) {
            tex = parse_texture(j.at("texture"));
        } else {
            tex = std::make_shared<SolidColor>(parse_color(j.at("albedo")));
        }
        double spec     = j.value("specular",     0.0);
        double spec_pow = j.value("specular_pow", 32.0);
        double refl     = j.value("reflection",   0.0);
        double trans    = j.value("transparency", 0.0);
        return std::make_shared<OpaqueMaterial>(tex, spec, spec_pow, refl, trans);
    }
    throw std::runtime_error("Unknown material type: " + type);
}

SceneConfig load_scene(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open scene file: " + path);

    json j;
    try {
        j = json::parse(file);
    } catch (const json::parse_error& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }

    SceneConfig cfg;

    // --- render settings ---
    const auto& render = j.at("render");
    cfg.width = render.value("width", 800);
    auto aspect = render.at("aspect_ratio");
    double ar = aspect[0].get<double>() / aspect[1].get<double>();
    cfg.height = static_cast<int>(cfg.width / ar);
    cfg.renderer.samples_per_pixel = render.value("samples_per_pixel", 4);
    cfg.renderer.max_depth         = render.value("max_depth", 10);
    cfg.renderer.sky_color         = parse_color(render.at("sky_color"));
    cfg.renderer.ambient           = parse_color(render.at("ambient"));

    // --- light ---
    const auto& light = j.at("light");
    cfg.renderer.light_dir   = unit_vector(parse_color(light.at("direction")));
    cfg.renderer.light_color = parse_color(light.at("color"));

    // --- camera ---
    const auto& cam = j.at("camera");
    cfg.camera.emplace(
        parse_color(cam.at("lookfrom")),
        parse_color(cam.at("lookat")),
        parse_color(cam.at("vup")),
        cam.at("vfov").get<double>(),
        ar,
        cam.value("aperture",    0.0),
        cam.value("focus_dist", 10.0)
    );

    // --- materials ---
    std::map<std::string, std::shared_ptr<Material>> materials;
    for (const auto& [name, def] : j.at("materials").items())
        materials[name] = parse_material(def);

    // --- objects ---
    for (const auto& obj : j.at("objects")) {
        std::string type = obj.at("type").get<std::string>();
        if (type == "sphere") {
            Point3 center = parse_color(obj.at("center"));
            double radius = obj.at("radius").get<double>();
            std::string mat_name = obj.at("material").get<std::string>();
            if (!materials.count(mat_name))
                throw std::runtime_error("Unknown material: " + mat_name);
            cfg.world.add(std::make_shared<Sphere>(center, radius, materials[mat_name]));
        } else {
            throw std::runtime_error("Unknown object type: " + type);
        }
    }

    // --- random spheres (optional) ---
    if (j.contains("random_spheres")) {
        const auto& rs = j.at("random_spheres");
        if (rs.value("enabled", false)) {
            int    grid_size        = rs.value("grid_size", 11);
            double radius           = rs.value("radius", 0.2);
            double reflective_chance = rs.value("reflective_chance", 0.3);

            for (int a = -grid_size; a < grid_size; a++) {
                for (int b = -grid_size; b < grid_size; b++) {
                    Point3 center(a + 0.9 * random_double(), radius, b + 0.9 * random_double());
                    if ((center - Point3(4, radius, 0)).length() <= 0.9) continue;

                    std::shared_ptr<Material> mat;
                    if (random_double() < reflective_chance) {
                        mat = std::make_shared<OpaqueMaterial>(Color::random(), 0.5, 32.0, 0.8);
                    } else {
                        mat = std::make_shared<OpaqueMaterial>(Color::random(), random_double());
                    }
                    cfg.world.add(std::make_shared<Sphere>(center, radius, mat));
                }
            }
        }
    }

    return cfg;
}
