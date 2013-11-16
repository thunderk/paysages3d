#ifndef ZONE_H
#define ZONE_H

#include "definition_global.h"

#include "BaseDefinition.h"

namespace paysages {
namespace definition {

class Circle;

class DEFINITIONSHARED_EXPORT Zone : public BaseDefinition
{
public:
    Zone(BaseDefinition *parent = 0);
    virtual ~Zone();

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void copy(BaseDefinition* destination) const override;

    void clear();

    void setAbsoluteHeight();
    void setRelativeHeight(double min, double middle, double max);

    void includeCircleArea(double value, double centerx, double centerz, double softradius, double hardradius);
    void excludeCircleArea(double centerx, double centerz, double softradius, double hardradius);

    void getHeightCurve(Curve* curve) const;
    void setHeightCurve(Curve* curve);
    void addHeightRangeQuick(double value, double hardmin, double softmin, double softmax, double hardmax);

    void getSlopeCurve(Curve* curve) const;
    void setSlopeCurve(Curve* curve);
    void addSlopeRangeQuick(double value, double hardmin, double softmin, double softmax, double hardmax);

    double getValue(const Vector3 &location, const Vector3 &normal) const;

private:
    int absolute_height;
    double relative_height_min;
    double relative_height_middle;
    double relative_height_max;

    Curve* value_by_height;
    Curve* value_by_slope;

    Circle* circles_included;
    int circles_included_count;
};

}
}

#endif // ZONE_H
