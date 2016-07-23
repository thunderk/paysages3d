#include "CameraDefinition.h"

#include "BoundingBox.h"
#include "Maths.h"
#include "PackStream.h"
#include "Scenery.h"
#include <cmath>

CameraDefinition::CameraDefinition(DefinitionNode *parent) : DefinitionNode(parent, "camera", "camera") {
    location.x = 0.0;
    location.y = 0.0;
    location.z = 0.0;
    direction.phi = 0.0;
    direction.theta = 0.0;
    direction.r = 1.0;
    roll = 0.0;

    width = 1.0;
    height = 1.0;
    perspective.yfov = 1.0;
    perspective.xratio = 1.0;
    perspective.znear = 0.5;
    perspective.zfar = Scenery::FAR_LIMIT_SCALED;

    validate();
}

void CameraDefinition::save(PackStream *stream) const {
    location.save(stream);
    stream->write(&direction.r);
    stream->write(&direction.phi);
    stream->write(&direction.theta);
    stream->write(&roll);
    stream->write(&perspective.yfov);
}

void CameraDefinition::load(PackStream *stream) {
    location.load(stream);
    stream->read(&direction.r);
    stream->read(&direction.phi);
    stream->read(&direction.theta);
    stream->read(&roll);
    stream->read(&perspective.yfov);

    validate();
}

void CameraDefinition::copy(DefinitionNode *_destination) const {
    CameraDefinition *destination = (CameraDefinition *)_destination;

    destination->location = location;
    destination->direction = direction;
    destination->roll = roll;

    destination->perspective = perspective;

    destination->validate();
}

void CameraDefinition::validate() {
    if (location.y > 300.0) {
        location.y = 300.0;
    }

    forward.x = 1.0;
    forward.y = 0.0;
    forward.z = 0.0;
    right.x = 0.0;
    right.y = 0.0;
    right.z = 1.0;
    up.x = 0.0;
    up.y = 1.0;
    up.z = 0.0;

    Matrix4 rotation = Matrix4::newRotateEuler(direction.phi, direction.theta, roll);

    forward = rotation.multPoint(forward);
    right = rotation.multPoint(right);
    up = rotation.multPoint(up);

    target = location.add(direction);

    Matrix4 mperspective =
        Matrix4::newPerspective(perspective.yfov, perspective.xratio, perspective.znear, perspective.zfar);
    unperspective = mperspective.inversed();

    projector = mperspective.mult(Matrix4::newLookAt(location, target, up));
    unprojector = projector.inversed();

    inv_x_factor = 1.0 / (0.5 * width);
    inv_y_factor = 1.0 / (0.5 * height);
}

double CameraDefinition::getRealDepth(const Vector3 &projected) const {
    Vector3 v(projected.x * inv_x_factor - 1.0, -(projected.y * inv_x_factor - 1.0), projected.z);
    return unperspective.transform(v).z;
}

void CameraDefinition::setLocation(const Vector3 &location) {
    this->location = location;

    validate();
}

void CameraDefinition::setLocationCoords(double x, double y, double z) {
    location = Vector3(x, y, z);

    validate();
}

void CameraDefinition::setTarget(const Vector3 &target) {
    Vector3 forward;

    forward = target.sub(location);
    if (forward.getNorm() < 0.0000001) {
        return;
    }

    direction = forward.toSpherical();

    validate();
}

void CameraDefinition::setTargetCoords(double x, double y, double z) {
    setTarget(Vector3(x, y, z));
}

void CameraDefinition::setRoll(double angle) {
    roll = angle;

    validate();
}

void CameraDefinition::setZoomToTarget(double zoom) {
    direction.r = zoom;
    location = target.add(Vector3(direction).scale(-1.0));

    validate();
}

void CameraDefinition::setFov(double fov) {
    perspective.yfov = fov;

    validate();
}

void CameraDefinition::strafeForward(double value) {
    location = location.add(forward.scale(value));

    validate();
}

void CameraDefinition::strafeRight(double value) {
    location = location.add(right.scale(value));

    validate();
}

void CameraDefinition::strafeUp(double value) {
    location = location.add(up.scale(value));

    validate();
}

void CameraDefinition::rotateYaw(double value) {
    direction.phi += value;

    validate();
}

void CameraDefinition::rotatePitch(double value) {
    direction.theta += value;

    validate();
}

void CameraDefinition::rotateRoll(double value) {
    roll += value;

    validate();
}

void CameraDefinition::setRenderSize(int width, int height) {
    this->width = to_double(width);
    this->height = to_double(height);
    perspective.xratio = this->width / this->height;

    validate();
}

Vector3 CameraDefinition::project(const Vector3 &point) const {
    Vector3 tpoint = projector.transform(point);
    if (tpoint.z < 1.0) {
        tpoint.x = -tpoint.x;
        tpoint.y = -tpoint.y;
    }
    tpoint.x = (tpoint.x + 1.0) * 0.5 * width;
    tpoint.y = (-tpoint.y + 1.0) * 0.5 * height;
    return tpoint;
}

Vector3 CameraDefinition::unproject(const Vector3 &point) const {
    Vector3 tpoint(point.x / (0.5 * width) - 1.0, -(point.y / (0.5 * height) - 1.0), point.z);
    if (tpoint.z < 1.0) {
        tpoint.x = -tpoint.x;
        tpoint.y = -tpoint.y;
    }
    return unprojector.transform(tpoint);
}

int CameraDefinition::isBoxInView(const Vector3 &center, double xsize, double ysize, double zsize) const {
    BoundingBox box;

    box.pushPoint(center.add(Vector3(-xsize, -ysize, -zsize)));
    box.pushPoint(center.add(Vector3(xsize, ysize, zsize)));

    return isUnprojectedBoxInView(box);
}

int CameraDefinition::isUnprojectedBoxInView(const BoundingBox &box) const {
    BoundingBox projected;

    projected.pushPoint(project(Vector3(box.xmin, box.ymin, box.zmin)));
    projected.pushPoint(project(Vector3(box.xmax, box.ymin, box.zmin)));
    projected.pushPoint(project(Vector3(box.xmin, box.ymax, box.zmin)));
    projected.pushPoint(project(Vector3(box.xmax, box.ymax, box.zmin)));
    projected.pushPoint(project(Vector3(box.xmin, box.ymin, box.zmax)));
    projected.pushPoint(project(Vector3(box.xmax, box.ymin, box.zmax)));
    projected.pushPoint(project(Vector3(box.xmin, box.ymax, box.zmax)));
    projected.pushPoint(project(Vector3(box.xmax, box.ymax, box.zmax)));

    return isProjectedBoxInView(projected);
}

int CameraDefinition::isProjectedBoxInView(const BoundingBox &box) const {
    if (box.xmin <= width && box.xmax >= 0.0 && box.ymin <= height && box.ymax >= 0.0 &&
        box.zmax >= perspective.znear) {
        double dx = box.xmax - box.xmin;
        double dy = box.ymax - box.ymin;

        return ceil_to_int(dx) * ceil_to_int(dy);
    } else {
        return 0;
    }
}

bool CameraDefinition::transitionToAnother(const CameraDefinition *wanted, double factor) {
    double dx, dy, dz, dr, dphi, dtheta, droll;

    dx = wanted->location.x - location.x;
    dy = wanted->location.y - location.y;
    dz = wanted->location.z - location.z;
    dr = wanted->direction.r - direction.r;
    dphi = Maths::modInRange(wanted->direction.phi - direction.phi, -Maths::PI, Maths::PI);
    dtheta = Maths::modInRange(wanted->direction.theta - direction.theta, -Maths::PI, Maths::PI);
    droll = Maths::modInRange(wanted->roll - roll, -Maths::PI, Maths::PI);

    if (fabs(dx) < 0.000001 && fabs(dy) < 0.000001 && fabs(dz) < 0.000001 && fabs(dr) < 0.000001 &&
        fabs(dphi) < 0.000001 && fabs(dtheta) < 0.000001 && fabs(droll) < 0.000001) {
        return false;
    } else {
        location.x += dx * factor;
        location.y += dy * factor;
        location.z += dz * factor;
        direction.r += dr * factor;
        direction.phi += dphi * factor;
        direction.theta += dtheta * factor;
        roll += droll * factor;

        validate();
        return true;
    }
}
