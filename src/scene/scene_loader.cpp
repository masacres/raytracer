#include "scene/scene_loader.h"
#include "core/utils.h"
#include "geometry/sphere.h"
#include "geometry/plane.h"
#include "geometry/cylinder.h"
#include "geometry/triangle.h"
#include "geometry/cuboid.h"
#include "geometry/torus.h"
#include "geometry/csg.h"
#include "materials/opaque_material.h"
#include "textures/checker.h"
#include "textures/uv_checker.h"
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
        return std::make_shared<XYZCheckerTexture>(c1, c2, freq);
    }
    if (type == "uv_checker") {
        Color c1      = parse_color(j.at("color1"));
        Color c2      = parse_color(j.at("color2"));
        double tiles_u = j.value("tiles_u", 2.0);
        double tiles_v = j.value("tiles_v", 2.0);
        return std::make_shared<UVCheckerTexture>(c1, c2, tiles_u, tiles_v);
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
    cfg.renderer.samples_per_pixel = 4;
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
    // inherited_mat: material propagated from a parent CSG node; nullptr at top level.
    auto parse_object = [&](auto& self,
                            const json& obj,
                            std::shared_ptr<Material> inherited_mat) -> std::shared_ptr<Hittable> {
        // Resolve material: explicit on this node wins, then inherited, then error.
        auto resolve_mat = [&]() -> std::shared_ptr<Material> {
            if (obj.contains("material")) {
                std::string mat_name = obj.at("material").get<std::string>();
                if (!materials.count(mat_name))
                    throw std::runtime_error("Unknown material: " + mat_name);
                return materials[mat_name];
            }
            if (inherited_mat) return inherited_mat;
            throw std::runtime_error("No material specified for object of type: " +
                                     obj.at("type").get<std::string>());
        };

        std::string type = obj.at("type").get<std::string>();
        if (type == "sphere") {
            Point3 center = parse_color(obj.at("center"));
            double radius = obj.at("radius").get<double>();
            return std::make_shared<Sphere>(center, radius, resolve_mat());
        } else if (type == "plane") {
            auto pt = parse_color(obj.at("point"));
            auto n  = parse_color(obj.at("normal"));
            return std::make_shared<Plane>(pt, n, resolve_mat());
        } else if (type == "cylinder") {
            Point3 cyl_center = parse_color(obj.at("center"));
            Vec3   cyl_axis   = parse_color(obj.at("axis"));
            double base_r     = obj.at("base_radius").get<double>();
            double top_r      = obj.value("top_radius", base_r);
            double cyl_height = obj.at("height").get<double>();
            return std::make_shared<Cylinder>(cyl_center, cyl_axis, base_r, top_r, cyl_height, resolve_mat());
        } else if (type == "triangle") {
            Point3 tp0 = parse_color(obj.at("p0"));
            Point3 tp1 = parse_color(obj.at("p1"));
            Point3 tp2 = parse_color(obj.at("p2"));
            return std::make_shared<Triangle>(tp0, tp1, tp2, resolve_mat());
        } else if (type == "cuboid") {
            Point3 cub_center = parse_color(obj.at("center"));
            Vec3   cub_u      = parse_color(obj.at("u"));
            Vec3   cub_v      = parse_color(obj.at("v"));
            double cub_w      = obj.at("width").get<double>();
            double cub_h      = obj.at("height").get<double>();
            double cub_d      = obj.at("depth").get<double>();
            return std::make_shared<Cuboid>(cub_center, cub_u, cub_v, cub_w, cub_h, cub_d, resolve_mat());
        } else if (type == "torus") {
            Point3 tor_center = parse_color(obj.at("center"));
            Vec3   tor_axis   = parse_color(obj.at("axis"));
            double tor_R      = obj.at("radius1").get<double>();
            double tor_r      = obj.at("radius2").get<double>();
            return std::make_shared<Torus>(tor_center, tor_axis, tor_R, tor_r, resolve_mat());
        } else if (type == "csg") {
            std::string op_str = obj.at("operation").get<std::string>();
            CSGOperation op;
            if      (op_str == "union")        op = CSGOperation::Union;
            else if (op_str == "intersection") op = CSGOperation::Intersection;
            else if (op_str == "difference")   op = CSGOperation::Difference;
            else throw std::runtime_error("Unknown CSG operation: " + op_str);
            // Material on this CSG node (if any) becomes the inherited material for children.
            std::shared_ptr<Material> child_mat = obj.contains("material") ? resolve_mat() : inherited_mat;
            auto left  = self(self, obj.at("left"),  child_mat);
            auto right = self(self, obj.at("right"), child_mat);
            return std::make_shared<CSGNode>(op, left, right);
        } else {
            throw std::runtime_error("Unknown object type: " + type);
        }
    };

    for (const auto& obj : j.at("objects"))
        cfg.world.add(parse_object(parse_object, obj, nullptr));

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
