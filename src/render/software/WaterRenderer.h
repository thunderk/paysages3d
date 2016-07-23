#pragma once

#include "software_global.h"

#include "LightFilter.h"

#include "Color.h"
#include "TerrainDefinition.h"
#include "Vector3.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT WaterRenderer : public LightFilter {
  public:
    typedef struct {
        Vector3 location;
        Color base;
        Color reflected;
        Color refracted;
        Color foam;
        Color final;
    } WaterResult;

  public:
    WaterRenderer(SoftwareRenderer *parent);
    virtual ~WaterRenderer();

    inline const FractalNoise &getNoise() const {
        return *noise;
    }

    virtual void update();

    virtual HeightInfo getHeightInfo();
    virtual double getHeight(double x, double z);
    virtual WaterResult getResult(double x, double z);
    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override;

  private:
    SoftwareRenderer *parent;
    FractalNoise *noise;
};
}
}
