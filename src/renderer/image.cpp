#include "renderer/image.h"
#include <fstream>
#include <stdexcept>

void Image::write_ppm(std::ostream& out, int samples_per_pixel) const {
    out << "P3\n" << width << ' ' << height << "\n255\n";
    // PPM rows go top-to-bottom; our pixels are stored bottom-to-top.
    for (int j = height - 1; j >= 0; j--)
        for (int i = 0; i < width; i++)
            write_color(out, at(i, j), samples_per_pixel);
}

void Image::save_ppm(const std::string& filename, int samples_per_pixel) const {
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open output file: " + filename);
    write_ppm(file, samples_per_pixel);
}
