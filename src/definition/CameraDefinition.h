#ifndef CAMERADEFINITION_H
#define CAMERADEFINITION_H

#include "definition_global.h"

#include "BaseDefinition.h"

#include "Vector3.h"
#include "Matrix4.h"

namespace paysages {
namespace definition {

typedef struct
{
    double yfov;
    double xratio;
    double znear;
    double zfar;
} CameraPerspective;

class BASICSSHARED_EXPORT CameraDefinition: public BaseDefinition
{
public:
    CameraDefinition();

    virtual void save(PackStream* pack) const override;
    virtual void load(PackStream* pack) override;

    virtual void copy(BaseDefinition* destination) const override;
    virtual void validate() override;

    inline Vector3 getLocation() const {return location;}
    inline Vector3 getTarget() const {return target;}
    inline Vector3 getUpVector() const {return up;}
    inline double getRoll() const {return roll;}
    inline Vector3 getDirection() const {return Vector3(direction);}
    inline Vector3 getDirectionNormalized() const {return forward;}
    inline const Matrix4 &getTransformationMatrix() const {return projector;}
    inline VectorSpherical getDirectionSpherical() const {return direction;}
    inline CameraPerspective getPerspective() const {return perspective;}

    double getRealDepth(const Vector3 &projected) const;

    void setLocation(const Vector3 &location);
    void setLocationCoords(double x, double y, double z);
    void setTarget(const Vector3 &target);
    void setTargetCoords(double x, double y, double z);
    void setRoll(double angle);
    void setZoomToTarget(double zoom);

    void strafeForward(double value);
    void strafeRight(double value);
    void strafeUp(double value);
    void rotateYaw(double value);
    void rotatePitch(double value);
    void rotateRoll(double value);

    void setRenderSize(int width, int height);
    Vector3 project(const Vector3 &point) const;
    Vector3 unproject(const Vector3 &point) const;
    bool isBoxInView(const Vector3 &center, double xsize, double ysize, double zsize) const;
    bool isUnprojectedBoxInView(const BoundingBox &box) const;
    bool isProjectedBoxInView(const BoundingBox &box) const;

    bool transitionToAnother(const CameraDefinition *wanted, double factor);

private:
    /* Definition */
    Vector3 location;
    VectorSpherical direction;
    double roll;

    /* Projection info */
    double width;
    double height;
    CameraPerspective perspective;

    /* Auto updated */
    Vector3 target;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    Matrix4 projector;
    Matrix4 unprojector;
    Matrix4 unperspective;
};

}
}

#endif // CAMERADEFINITION_H
