#ifndef ATMOSPHERERESULT_H
#define ATMOSPHERERESULT_H

#include "software_global.h"

#include "Color.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT AtmosphereResult
{
public:
    AtmosphereResult();
    void updateFinal();

    Color base;
    double distance;
    Color inscattering;
    Color attenuation;
    Color mask;
    Color final;
};

}
}

#endif // ATMOSPHERERESULT_H
