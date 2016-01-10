#include "Vector3.h"

#include <cmath>
#include "Maths.h"
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

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {
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

Vector3 Vector3::add(double x, double y, double z) const {
    return Vector3(this->x + x, this->y + y, this->z + z);
}

Vector3 Vector3::add(const Vector3 &other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::sub(const Vector3 &other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::inverse() const {
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::scale(double scaling) const {
    return Vector3(x * scaling, y * scaling, z * scaling);
}

double Vector3::getNorm() const {
    return sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::normalize() const {
    double norm = sqrt(x * x + y * y + z * z);
    if (norm == 0.0) {
        return VECTOR_ZERO;
    } else {
        norm = 1.0 / norm;
        return Vector3(x * norm, y * norm, z * norm);
    }
}

double Vector3::dotProduct(const Vector3 &other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::crossProduct(const Vector3 &other) const {
    return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

static inline double _euclidGet2DAngle(double x, double y) {
    // TEMP Copy of old euclid.c
    double nx, ny, d, ret;

    if (x == 0.0) {
        if (y == 0.0) {
            return 0.0;
        } else if (y < 0.0) {
            return 3.0 * Maths::PI_2;
        } else {
            return Maths::PI_2;
        }
    }

    d = sqrt(x * x + y * y);
    nx = x / d;
    ny = y / d;

    ret = asin(ny);
    if (nx < 0.0) {
        ret = Maths::PI - ret;
    }
    return ret < 0.0 ? ret + 2.0 * Maths::PI : ret;
}

VectorSpherical Vector3::toSpherical() const {
    VectorSpherical result;

    result.phi = _euclidGet2DAngle(x, -z);
    result.theta = _euclidGet2DAngle(sqrt(x * x + z * z), y);
    if (y < 0.0) {
        result.theta -= 2.0 * Maths::PI;
    }
    result.r = getNorm();

    return result;
}

Vector3 Vector3::getNormal3(const Vector3 &posx, const Vector3 &posy) const {
    return posx.sub(*this).crossProduct(posy.sub(*this)).normalize();
}

Vector3 Vector3::getNormal5(const Vector3 &posx, const Vector3 &negx, const Vector3 &posy, const Vector3 &negy) const {
    Vector3 dnegx = negx.sub(*this);
    Vector3 dposy = posy.sub(*this);
    Vector3 dposx = posx.sub(*this);
    Vector3 dnegy = negy.sub(*this);

    Vector3 normal = dposy.crossProduct(dnegx);
    normal = normal.add(dposx.crossProduct(dposy));
    normal = normal.add(dnegy.crossProduct(dposx));
    normal = normal.add(dnegx.crossProduct(dnegy));

    return normal.normalize();
}

Vector3 Vector3::midPointTo(const Vector3 &other) const {
    return Vector3((other.x + x) * 0.5, (other.y + y) * 0.5, (other.z + z) * 0.5);
}

Vector3 Vector3::randomInSphere(double radius, bool only_surface, RandomGenerator &random) {
    // TODO More uniform spatial repartition
    //  The current randomization clusters result near the center and at the poles
    VectorSpherical vec = {only_surface ? radius : random.genDouble() * radius, (random.genDouble() - 0.5) * Maths::PI,
                           random.genDouble() * Maths::TWOPI};
    return Vector3(vec);
}
