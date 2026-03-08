# Raytracer

A C++17 path tracer built from scratch, featuring physically-based materials, BVH acceleration, and depth of field.

![Example render](output/render.ppm)

## Features

- **Materials** — Lambertian (diffuse), Metal (reflective + fuzzy), Dielectric (glass)
- **Textures** — Solid color, Checker
- **Acceleration** — Bounding Volume Hierarchy (BVH)
- **Camera** — Configurable FOV, look-at, depth of field
- **Anti-aliasing** — Multi-sample per pixel
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
│   │   ├── material.h       # Abstract Material
│   │   ├── lambertian.h/.cpp
│   │   ├── metal.h/.cpp
│   │   └── dielectric.h/.cpp
│   ├── textures/
│   │   ├── texture.h        # Abstract Texture
│   │   ├── solid_color.h/.cpp
│   │   └── checker.h/.cpp
│   ├── acceleration/
│   │   └── bvh.h/.cpp       # BVH tree
│   └── renderer/
│       ├── renderer.h/.cpp  # Main render loop
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
| `image_width` | `400` | Output width in pixels |
| `aspect_ratio` | `16:9` | Aspect ratio |
| `samples_per_pixel` | `50` | Rays per pixel (higher = less noise) |
| `max_depth` | `50` | Max ray bounces |
| `aperture` | `0.1` | Lens aperture (0 = no blur) |
| Camera position | `(13,2,3)` | `lookfrom` in `main()` |

## Adding Objects

```cpp
// Diffuse sphere
world.add(std::make_shared<Sphere>(
    Point3(0, 0.5, -2), 0.5,
    std::make_shared<Lambertian>(Color(0.8, 0.2, 0.2))));

// Mirror sphere
world.add(std::make_shared<Sphere>(
    Point3(1, 0.5, -2), 0.5,
    std::make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.0)));

// Glass sphere
world.add(std::make_shared<Sphere>(
    Point3(-1, 0.5, -2), 0.5,
    std::make_shared<Dielectric>(1.5)));
```

## References

- [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html) — Peter Shirley
- [_Ray Tracing: The Next Week_](https://raytracing.github.io/books/RayTracingTheNextWeek.html) — Peter Shirley
