# Raytracer

A C++17 raytracer built from scratch, featuring a Phong direct-lighting model, procedural textures, and BVH acceleration.

![Example render](render.png)

## Features

- **Materials** — `OpaqueMaterial` with albedo, specular coefficient, shininess exponent, and mirror reflection
- **Lighting** — Directional light with ambient, diffuse (Lambertian), and specular (Phong) terms; hard shadows
- **Textures** — Solid color, Checker
- **Acceleration** — Bounding Volume Hierarchy (BVH)
- **Camera** — Configurable FOV, look-at, depth of field
- **Anti-aliasing** — 4-sample fixed grid per pixel
- **Gamma correction** — γ = 2.0
- **Output** — PPM image format
- **JSON scenes** — Scene, camera, materials, and objects defined in a JSON file; no recompile needed to change the scene

## Project Structure

```
raytracer/
├── CMakeLists.txt
├── scenes/
│   └── default.json         # Scene definition (camera, lights, materials, objects)
├── third_party/
│   └── nlohmann/
│       └── json.hpp         # JSON parser (single-header, must be provided)
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── utils.h          # Constants, random numbers, clamp
│   │   ├── vec3.h           # Vec3 / Point3 / Color + math
│   │   ├── ray.h            # Ray class
│   │   ├── color.h          # write_color utility
│   │   └── camera.h/.cpp    # Configurable camera with DoF
│   ├── geometry/
│   │   ├── aabb.h           # Axis-aligned bounding box
│   │   ├── hittable.h       # Abstract Hittable + HitRecord
│   │   ├── hittable_list.h/.cpp
│   │   └── sphere.h/.cpp
│   ├── materials/
│   │   ├── material.h       # Abstract Material (get_specular, get_specular_pow, get_reflection)
│   │   └── opaque_material.h/.cpp
│   ├── textures/
│   │   ├── texture.h        # Abstract Texture
│   │   ├── solid_color.h/.cpp
│   │   └── checker.h/.cpp
│   ├── acceleration/
│   │   └── bvh.h/.cpp       # BVH tree
│   ├── scene/
│   │   └── scene_loader.h/.cpp  # Loads SceneConfig from a JSON file
│   └── renderer/
│       ├── renderer.h/.cpp  # Main render loop + Phong shading
│       └── image.h/.cpp     # PPM image output
└── output/                  # Rendered images saved here
```

## Dependencies

- CMake 3.16+, C++17 compiler
- [nlohmann/json](https://github.com/nlohmann/json) — place `json.hpp` at `third_party/nlohmann/json.hpp` before building

## Build

```bash
cmake -B build
cmake --build build
```

## Run

```bash
./build/raytracer                      # uses scenes/default.json
./build/raytracer scenes/default.json  # explicit scene path
```

Output is written to `output/render.ppm`. Open it in any PPM-capable viewer (GIMP, feh, Preview on macOS).

## Scene File

All scene data lives in `scenes/default.json`. Edit it and re-run — no recompile needed.

```json
{
  "render": {
    "width": 800,
    "aspect_ratio": [16, 9],
    "samples_per_pixel": 4,
    "max_depth": 10,
    "sky_color": [0.5, 0.5, 0.8],
    "ambient": [0.05, 0.05, 0.05]
  },
  "light": {
    "direction": [-1.0, -2.0, -1.0],
    "color": [0.9, 0.9, 0.9]
  },
  "camera": {
    "lookfrom": [13, 2, 3],
    "lookat": [0, 0, 0],
    "vup": [0, 1, 0],
    "vfov": 20.0,
    "aperture": 0.1,
    "focus_dist": 10.0
  },
  "materials": {
    "my_mat": {
      "type": "opaque",
      "albedo": [0.8, 0.2, 0.2],
      "specular": 0.5,
      "specular_pow": 32.0,
      "reflection": 0.0
    },
    "checker_mat": {
      "type": "opaque",
      "texture": { "type": "checker", "color1": [0.2, 0.3, 0.1], "color2": [0.9, 0.9, 0.9], "frequency": 5 },
      "specular": 0.4
    }
  },
  "objects": [
    { "type": "sphere", "center": [0, 0.5, -2], "radius": 0.5, "material": "my_mat" }
  ],
  "random_spheres": {
    "enabled": true,
    "grid_size": 11,
    "radius": 0.2,
    "reflective_chance": 0.3
  }
}
```

### Material fields (`"type": "opaque"`)

| Field | Default | Description |
|---|---|---|
| `albedo` | — | Base color as `[r, g, b]`; use instead of `texture` |
| `texture` | — | Procedural texture object; use instead of `albedo` |
| `specular` | `0.0` | Specular highlight intensity in [0, 1] |
| `specular_pow` | `32.0` | Phong shininess exponent (higher = tighter highlight) |
| `reflection` | `0.0` | Mirror reflection weight in [0, 1] |

### Texture types

| Type | Fields |
|---|---|
| `"solid"` | `"color": [r, g, b]` |
| `"checker"` | `"color1"`, `"color2"`, `"frequency"` |

## Third-Party Licenses

- **nlohmann/json** — MIT License, Copyright (c) 2013-2022 Niels Lohmann. See https://github.com/nlohmann/json/blob/develop/LICENSE.MIT

## Development

Built with assistance from [Claude Code](https://claude.ai/code) (Anthropic).

## References

- [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html) — Peter Shirley
- [_Ray Tracing: The Next Week_](https://raytracing.github.io/books/RayTracingTheNextWeek.html) — Peter Shirley
