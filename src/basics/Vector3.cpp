#include "Vector3.inline.cpp"

#include <cmath>
#include "PackStream.h"
#include "RandomGenerator.h"

const Vector3 paysages::basics::VECTOR_ZERO(0.0, 0.0, 0.0);
const Vector3 paysages::basics::VECTOR_DOWN(0.0, -1.0, 0.0);
const Vector3 paysages::basics::VECTOR_UP(0.0, 1.0, 0.0);
const Vector3 paysages::basics::VECTOR_NORTH(0.0, 0.0, -1.0);
const Vector3 paysages::basics::VECTOR_SOUTH(0.0, 0.0, 1.0);
const Vector3 paysages::basics::VECTOR_WEST(-1.0, 0.0, 0.0);
const Vector3 paysages::basics::VECTOR_EAST(1.0, 0.0, 0.0);

Vector3::Vector3(const VectorSpherical &v)
    : x(v.r * cos(v.phi) * cos(v.theta)), y(v.r * sin(v.theta)), z(-v.r * sin(v.phi) * cos(v.theta)) {
}

void Vector3::save(PackStream *stream) const {
    stream->write(&x);
    stream->write(&y);
    stream->write(&z);
}
void Vector3::load(PackStream *stream) {
    stream->read(&x);
    stream->read(&y);
    stream->read(&z);
}

static inline double _euclidGet2DAngle(double x, double y) {
    // TEMP Copy of old euclid.c
    double nx, ny, d, ret;

    if (x == 0.0) {
        if (y == 0.0) {
            return 0.0;
        } else if (y < 0.0) {
            return 3.0 * M_PI_2;
        } else {
            return M_PI_2;
        }
    }

    d = sqrt(x * x + y * y);
    nx = x / d;
    ny = y / d;

    ret = asin(ny);
    if (nx < 0.0) {
        ret = M_PI - ret;
    }
    return ret < 0.0 ? ret + 2.0 * M_PI : ret;
}

VectorSpherical Vector3::toSpherical() const {
    VectorSpherical result;

    result.phi = _euclidGet2DAngle(x, -z);
    result.theta = _euclidGet2DAngle(sqrt(x * x + z * z), y);
    if (y < 0.0) {
        result.theta -= 2.0 * M_PI;
    }
    result.r = getNorm();

    return result;
}

Vector3 Vector3::midPointTo(const Vector3 &other) const {
    return Vector3((other.x + x) * 0.5, (other.y + y) * 0.5, (other.z + z) * 0.5);
}

Vector3 Vector3::randomInSphere(double radius, bool only_surface) {
    // TODO More uniform spatial repartition
    //  The current randomization clusters result near the center and at the poles
    VectorSpherical vec = {only_surface ? radius : RandomGenerator::random() * radius,
                           (RandomGenerator::random() - 0.5) * M_PI, RandomGenerator::random() * M_2PI};
    return Vector3(vec);
}
