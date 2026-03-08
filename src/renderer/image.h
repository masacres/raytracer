#pragma once

#include "core/color.h"
#include <string>
#include <vector>
#include <ostream>

class Image {
public:
    int width;
    int height;
    std::vector<Color> pixels;  // row-major, (0,0) = bottom-left

    Image(int w, int h) : width(w), height(h), pixels(w * h, Color(0,0,0)) {}

    Color&       at(int x, int y)       { return pixels[y * width + x]; }
    const Color& at(int x, int y) const { return pixels[y * width + x]; }

    void write_ppm(std::ostream& out, int samples_per_pixel) const;
    void save_ppm(const std::string& filename, int samples_per_pixel) const;
};
