#include "Sphere.h"

#include "InfiniteRay.h"
#include "PackStream.h"
#include <cmath>

Sphere::Sphere(const Vector3 &center, double radius) : center(center), radius(radius) {
    radius2 = radius * radius;
}

int Sphere::checkRayIntersection(const InfiniteRay &ray) const {
    Vector3 L = ray.getOrigin().sub(center);
    double b = 2.0 * ray.getDirection().dotProduct(L);
    double c = L.dotProduct(L) - radius2;

    double discr = b * b - 4.0 * c;
    if (discr < 0) {
        return 0;
    } else if (discr == 0) {
        return 1;
    } else {
        return 2;
    }
}

int Sphere::findRayIntersection(const InfiniteRay &ray, Vector3 *first_intersection,
                                Vector3 *second_intersection) const {
    Vector3 L = ray.getOrigin().sub(center);
    double b = 2.0 * ray.getDirection().dotProduct(L);
    double c = L.dotProduct(L) - radius2;

    double discr = b * b - 4.0 * c;
    if (discr < 0) {
        return 0;
    } else if (discr == 0) {
        *first_intersection = ray.getPointAtCursor(-0.5 * b);
        return 1;
    } else {
        double x0 = (b > 0.0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
        double x1 = c / x0;
        if (x0 > x1) {
            *first_intersection = ray.getPointAtCursor(x1);
            *second_intersection = ray.getPointAtCursor(x0);
        } else {
            *first_intersection = ray.getPointAtCursor(x0);
            *second_intersection = ray.getPointAtCursor(x1);
        }
        return 2;
    }
}

void Sphere::save(PackStream *stream) const {
    center.save(stream);
    stream->write(&radius);
    stream->write(&radius2);
}

void Sphere::load(PackStream *stream) {
    center.load(stream);
    stream->read(&radius);
    stream->read(&radius2);
}
