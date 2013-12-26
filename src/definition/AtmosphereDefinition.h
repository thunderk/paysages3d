#ifndef ATMOSPHEREDEFINITION_H
#define ATMOSPHEREDEFINITION_H

#include "definition_global.h"

#include "BaseDefinition.h"

#include "Vector3.h"
#include "Color.h"

namespace paysages {
namespace definition {

class AtmosphereDefinition : public BaseDefinition
{
public:
    typedef struct
    {
        Vector3 location;
        double radius;
        Color col;
    } Star;

public:
    typedef enum
    {
        ATMOSPHERE_MODEL_DISABLED = 0,
        ATMOSPHERE_MODEL_BRUNETON = 1
    } AtmosphereModel;
    typedef enum
    {
        ATMOSPHERE_PRESET_CLEAR_DAY = 0,
        ATMOSPHERE_PRESET_CLEAR_SUNSET = 1,
        ATMOSPHERE_PRESET_HAZY_MORNING = 2,
        ATMOSPHERE_PRESET_FOGGY = 3,
        ATMOSPHERE_PRESET_STORMY = 4
    } AtmospherePreset;

public:
    AtmosphereDefinition(BaseDefinition* parent);
    virtual ~AtmosphereDefinition();

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void copy(BaseDefinition* destination) const override;
    virtual void validate() override;

    void applyPreset(AtmospherePreset preset);
    void generateStars(int count);

public:
    AtmosphereModel model;
    int hour;
    int minute;
    double humidity;
    Color sun_color;
    double sun_radius;
    double dome_lighting;

    double moon_radius;
    double moon_theta;
    double moon_phi;

    double _daytime;

    std::vector<Star> stars;
};

}
}

#endif // ATMOSPHEREDEFINITION_H
