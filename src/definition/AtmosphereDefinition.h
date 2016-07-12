#pragma once

#include "definition_global.h"

#include "DefinitionNode.h"

#include "Vector3.h"
#include "Color.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT AtmosphereDefinition : public DefinitionNode {
  public:
    typedef struct {
        Vector3 location;
        double radius;
        Color col;
    } Star;

  public:
    typedef enum { ATMOSPHERE_MODEL_DISABLED = 0, ATMOSPHERE_MODEL_BRUNETON = 1 } AtmosphereModel;
    typedef enum {
        ATMOSPHERE_PRESET_CLEAR_DAY = 0,
        ATMOSPHERE_PRESET_CLEAR_SUNSET = 1,
        ATMOSPHERE_PRESET_HAZY_MORNING = 2,
        ATMOSPHERE_PRESET_FOGGY = 3,
        ATMOSPHERE_PRESET_STORMY = 4
    } AtmospherePreset;

  public:
    AtmosphereDefinition(DefinitionNode *parent);

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;

    virtual void copy(DefinitionNode *destination) const override;

    inline GodRaysDefinition *childGodRays() const {
        return godrays;
    }
    inline CelestialBodyDefinition *childSun() const {
        return sun;
    }
    inline CelestialBodyDefinition *childMoon() const {
        return moon;
    }
    inline FloatNode *propHumidity() const {
        return humidity;
    }

    /**
     * Set the daytime from a 0.0-1.0 value.
     */
    void setDayTime(double value);
    /**
     * Set the daytime from hour/minute/second info.
     */
    void setDayTime(int hour, int minute = 0, int second = 0);
    /**
     * Get the daytime info, in 0.0-1.0 value.
     */
    double getDaytime() const;
    /**
     * Get the daytime info, in hour/minute/second.
     */
    void getHMS(int *hour, int *minute, int *second) const;

    void applyPreset(AtmospherePreset preset, RandomGenerator &random = RandomGeneratorDefault);
    void generateStars(int count, RandomGenerator &random = RandomGeneratorDefault);

  public:
    AtmosphereModel model;

    Color sun_color;
    double dome_lighting;

    vector<Star> stars;

  private:
    GodRaysDefinition *godrays;
    CelestialBodyDefinition *sun;
    CelestialBodyDefinition *moon;
    FloatNode *humidity;
};
}
}
