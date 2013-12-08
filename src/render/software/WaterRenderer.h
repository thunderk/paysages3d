#ifndef WATERRENDERER_H
#define WATERRENDERER_H

#include "software_global.h"

#include "TerrainDefinition.h"
#include "Vector3.h"
#include "Color.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT WaterRenderer
{
public:
    typedef struct
    {
        Vector3 location;
        Color base;
        Color reflected;
        Color refracted;
        Color foam;
        Color final;
    } WaterResult;

public:
    WaterRenderer(SoftwareRenderer* parent);
    virtual ~WaterRenderer();

    virtual HeightInfo getHeightInfo();
    virtual double getHeight(double x, double z);
    virtual WaterResult getResult(double x, double z);
    virtual int alterLight(LightComponent *light, const Vector3 &at);

private:
    SoftwareRenderer* parent;
};

}
}

#endif // WATERRENDERER_H
