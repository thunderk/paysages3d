#ifndef WATERMODELER_H
#define WATERMODELER_H

#include "modeler_global.h"

namespace paysages {
namespace modeler {

class WaterModeler
{
public:
    WaterModeler(MainModelerWindow *main);
    ~WaterModeler();

private:
    FloatPropertyBind *prop_water_height;
};

}
}

#endif // WATERMODELER_H
