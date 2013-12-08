#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "software_global.h"

#include "Color.h"
#include "Vector3.h"

namespace paysages {
namespace software {

/**
 * @brief One component of a LightStatus.
 *
 * A light component represents the amount of light received at a point from a given direction.
 */
class SOFTWARESHARED_EXPORT LightComponent
{
public:
    //LightComponent();

    Color color;        // Light power
    Vector3 direction;  // Direction the light is travelling
    double reflection;  /* Reflected factor of the light (for specular lighting) */
    bool altered;       /* Should the light be filtered or masked (by atmosphere, water, ground, clouds...) */
};

}
}

#endif // LIGHTCOMPONENT_H
