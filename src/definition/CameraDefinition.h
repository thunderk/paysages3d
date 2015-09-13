#ifndef CAMERADEFINITION_H
#define CAMERADEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"

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

class DEFINITIONSHARED_EXPORT CameraDefinition: public DefinitionNode
{
public:
    CameraDefinition(DefinitionNode *parent = NULL);

    virtual void save(PackStream* pack) const override;
    virtual void load(PackStream* pack) override;

    virtual void copy(DefinitionNode* destination) const override;
    virtual void validate() override;

    inline const Vector3 &getLocation() const {return location;}
    inline const Vector3 &getTarget() const {return target;}
    inline const Vector3 &getUpVector() const {return up;}
    inline double getRoll() const {return roll;}
    inline Vector3 getDirection() const {return Vector3(direction);}
    inline const Vector3 &getDirectionNormalized() const {return forward;}
    inline const Matrix4 &getTransformationMatrix() const {return projector;}
    inline const VectorSpherical &getDirectionSpherical() const {return direction;}
    inline const CameraPerspective &getPerspective() const {return perspective;}
    inline double getWidth() const {return width;}
    inline double getHeight() const {return height;}

    double getRealDepth(const Vector3 &projected) const;

    void setLocation(const Vector3 &location);
    void setLocationCoords(double x, double y, double z);
    void setTarget(const Vector3 &target);
    void setTargetCoords(double x, double y, double z);
    void setRoll(double angle);
    void setZoomToTarget(double zoom);
    void setFov(double fov);

    void strafeForward(double value);
    void strafeRight(double value);
    void strafeUp(double value);
    void rotateYaw(double value);
    void rotatePitch(double value);
    void rotateRoll(double value);

    void setRenderSize(int width, int height);
    Vector3 project(const Vector3 &point) const;
    Vector3 unproject(const Vector3 &point) const;
    int isBoxInView(const Vector3 &center, double xsize, double ysize, double zsize) const;
    int isUnprojectedBoxInView(const BoundingBox &box) const;
    int isProjectedBoxInView(const BoundingBox &box) const;

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
    double inv_x_factor;
    double inv_y_factor;
};

}
}

#endif // CAMERADEFINITION_H
