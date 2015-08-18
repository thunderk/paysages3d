#ifndef ATMOSPHEREDEFINITION_H
#define ATMOSPHEREDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"

#include "Vector3.h"
#include "Color.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT AtmosphereDefinition : public DefinitionNode
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
    AtmosphereDefinition(DefinitionNode* parent);
    virtual ~AtmosphereDefinition();

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void copy(DefinitionNode* destination) const override;

    inline FloatNode *propDayTime() const {return daytime;}

    /**
     * Set the daytime from a 0.0-1.0 value.
     */
    void setDayTime(double value);
    /**
     * Set the daytime from hour/minute/second info.
     */
    void setDayTime(int hour, int minute=0, int second=0);
    /**
     * Get the daytime info, in hour/minute/second.
     */
    void getHMS(int *hour, int *minute, int *second) const;

    void applyPreset(AtmospherePreset preset);
    void generateStars(int count);

public:
    AtmosphereModel model;
    double humidity;
    Color sun_color;
    double sun_radius;
    double dome_lighting;

    double moon_radius;
    double moon_theta;
    double moon_phi;

    std::vector<Star> stars;

private:
    FloatNode *daytime;
};

}
}

#endif // ATMOSPHEREDEFINITION_H
