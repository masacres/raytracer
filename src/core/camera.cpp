#include "core/camera.h"
#include <cmath>

Camera::Camera(Point3 lookfrom, Point3 lookat, Vec3 vup,
               double vfov, double aspect_ratio,
               double aperture, double focus_dist) {
    double h               = std::tan(degrees_to_radians(vfov) / 2.0);
    double viewport_height = 2.0 * h;
    double viewport_width  = aspect_ratio * viewport_height;

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    origin            = lookfrom;
    horizontal        = focus_dist * viewport_width  * u;
    vertical          = focus_dist * viewport_height * v;
    lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;
    lens_radius       = aperture / 2.0;
}

Ray Camera::get_ray(double s, double t) const {
    return Ray(
        origin,
        lower_left_corner + s*horizontal + t*vertical - origin
    );
}
