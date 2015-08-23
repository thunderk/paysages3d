#ifndef ATMOSPHEREMODELER_H
#define ATMOSPHEREMODELER_H

#include "modeler_global.h"

namespace paysages {
namespace modeler {

class AtmosphereModeler
{
public:
    AtmosphereModeler(MainModelerWindow *main);
    ~AtmosphereModeler();

private:
    FloatPropertyBind *prop_daytime;
    FloatPropertyBind *prop_humidity;
};

}
}

#endif // ATMOSPHEREMODELER_H
