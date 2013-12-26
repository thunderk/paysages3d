#ifndef NIGHTSKY_H
#define NIGHTSKY_H

#include "software_global.h"

namespace paysages {
namespace software {

/*!
 * \brief Night sky renderer.
 */
class SOFTWARESHARED_EXPORT NightSky
{
public:
    NightSky(SoftwareRenderer* renderer);
    virtual ~NightSky();

    /*!
     * \brief Update the night sky renderer, when the scenery or parent renderer changed.
     */
    void update();

    /*!
     * \brief Get the color of the night sky at a given direction.
     * \param altitude Altitude above water level, in coordinate units (not kilometers).
     */
    virtual const Color getColor(double altitude, const Vector3 &direction);

private:
    SoftwareRenderer* renderer;
};

}
}

#endif // NIGHTSKY_H
