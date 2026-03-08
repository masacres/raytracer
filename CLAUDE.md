# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
cmake -B build && cmake --build build   # configure + build
./build/raytracer                        # render to output/render.ppm
```

No test framework is set up yet. Validate changes by building and inspecting the rendered PPM output.

## Architecture

All includes use paths **relative to `src/`**, which is the sole CMake include root. Every header uses `#pragma once`.

### Key abstractions and their relationships

**`Hittable` / `HitRecord`** (`geometry/hittable.h`)
The core interface. Every scene object implements `hit()` and `bounding_box()`. `HitRecord` carries the hit point, shading normal (always against the ray), `t`, and a `shared_ptr<Material>`. `Material` is only forward-declared here to break the circular dependency — `material.h` includes `hittable.h`, not the other way around.

**`Material`** (`materials/material.h`)
Single virtual method: `scatter(r_in, rec, attenuation, scattered)`. Returns false to terminate the path. Implementations: `Lambertian`, `Metal`, `Dielectric`.

**`Texture`** (`textures/texture.h`)
`value(u, v, p) → Color`. `Lambertian` holds a `shared_ptr<Texture>` so it accepts either a flat color (`SolidColor`) or a procedural pattern (`CheckerTexture`). Other materials take `Color` directly.

**`Vec3`** (`core/vec3.h`)
Type-aliased as both `Point3` and `Color`. All math helpers (`reflect`, `refract`, `random_in_unit_sphere`, etc.) are inline free functions in the same header. `core/utils.h` provides `random_double`, `clamp`, and constants — it must be included before `vec3.h` since `vec3.h` depends on it.

**`Renderer`** (`renderer/renderer.h`)
Owns `samples_per_pixel` and `max_depth`. `render()` returns an `Image` (row-major, y=0 at bottom). `ray_color()` is the recursive path-tracing kernel; it hits `t_min = 0.001` to avoid self-intersection.

**`BVHNode`** (`acceleration/bvh.h`)
Built from a `HittableList`. Recursively splits on a random axis. Wrap a `HittableList` in a `BVHNode` before passing to the renderer to get the acceleration benefit.

### Adding a new primitive

1. Subclass `Hittable` in `geometry/`.
2. Implement `hit()` — call `rec.set_face_normal()` to set the shading normal.
3. Implement `bounding_box()` — required for BVH; return false if unbounded.
4. Add to the scene in `main.cpp` via `world.add(std::make_shared<YourPrimitive>(...))`.

### Adding a new material

1. Subclass `Material` in `materials/`, include `materials/material.h`.
2. Implement `scatter()` — write the outgoing ray into `scattered` and the throughput into `attenuation`.

### Output format

`Image::save_ppm()` writes a plain PPM (P3). Rows are written top-to-bottom; pixels are stored bottom-to-top internally. To add PNG support, add `stb_image_write.h` to `third_party/` and call it from `image.cpp`.
