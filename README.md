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

## Project Structure

```
raytracer/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── utils.h          # Constants, random numbers, clamp
│   │   ├── vec3.h           # Vec3 / Point3 / Color + math
│   │   ├── ray.h            # Ray class
│   │   ├── color.h          # write_color utility
│   │   ├── camera.h/.cpp    # Configurable camera with DoF
│   ├── geometry/
│   │   ├── aabb.h           # Axis-aligned bounding box
│   │   ├── hittable.h       # Abstract Hittable + HitRecord
│   │   ├── hittable_list.h/.cpp
│   │   └── sphere.h/.cpp
│   ├── materials/
│   │   ├── material.h       # Abstract Material (get_specular, get_specular_pow)
│   │   └── opaque_material.h/.cpp
│   ├── textures/
│   │   ├── texture.h        # Abstract Texture
│   │   ├── solid_color.h/.cpp
│   │   └── checker.h/.cpp
│   ├── acceleration/
│   │   └── bvh.h/.cpp       # BVH tree
│   └── renderer/
│       ├── renderer.h/.cpp  # Main render loop + Phong shading
│       └── image.h/.cpp     # PPM image output
└── output/                  # Rendered images saved here
```

## Build

Requires CMake 3.16+ and a C++17 compiler.

```bash
cmake -B build
cmake --build build
```

## Run

```bash
./build/raytracer
# output written to output/render.ppm
```

Open `output/render.ppm` in any image viewer that supports PPM (e.g. GIMP, feh, Preview on macOS).

## Configuration

Edit `src/main.cpp` to adjust:

| Setting | Default | Description |
|---|---|---|
| `image_width` | `800` | Output width in pixels |
| `aspect_ratio` | `16:9` | Aspect ratio |
| `max_depth` | `10` | Max ray bounces |
| `aperture` | `0.1` | Lens aperture (0 = no blur) |
| Camera position | `(13,2,3)` | `lookfrom` in `main()` |

Renderer light settings (set on the `Renderer` object in `main()`):

| Field | Default | Description |
|---|---|---|
| `light_dir` | `(-1,-2,-1)` normalized | Directional light direction |
| `light_color` | `(0.9, 0.9, 0.9)` | Light color/intensity |
| `ambient` | `(0.05, 0.05, 0.05)` | Ambient fill color |
| `sky_color` | `(0.5, 0.5, 0.8)` | Background/miss color |

## Adding Objects

```cpp
// Matte sphere (no specular)
world.add(std::make_shared<Sphere>(
    Point3(0, 0.5, -2), 0.5,
    std::make_shared<OpaqueMaterial>(Color(0.8, 0.2, 0.2))));

// Shiny sphere (high specular, tight highlight)
world.add(std::make_shared<Sphere>(
    Point3(1, 0.5, -2), 0.5,
    std::make_shared<OpaqueMaterial>(Color(0.8, 0.8, 0.8), 0.9, 64.0)));

// Reflective sphere (mirror-like environment reflection)
world.add(std::make_shared<Sphere>(
    Point3(0, 0.5, -2), 0.5,
    std::make_shared<OpaqueMaterial>(Color(0.5, 0.6, 0.6), 0.8, 16.0, 0.7)));

// Checker-textured sphere
auto tex = std::make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
world.add(std::make_shared<Sphere>(
    Point3(-1, 0.5, -2), 0.5,
    std::make_shared<OpaqueMaterial>(tex)));
```

`OpaqueMaterial` constructor: `(albedo, specular = 0.0, specular_pow = 32.0, reflection = 0.0)`
- `specular` — highlight intensity in [0, 1]
- `specular_pow` — Phong shininess exponent (higher = tighter highlight)
- `reflection` — mirror reflection weight in [0, 1] (0 = matte, 1 = perfect mirror)

## References

- [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html) — Peter Shirley
- [_Ray Tracing: The Next Week_](https://raytracing.github.io/books/RayTracingTheNextWeek.html) — Peter Shirley
