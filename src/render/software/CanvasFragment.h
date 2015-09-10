#ifndef CANVASFRAGMENT_H
#define CANVASFRAGMENT_H

#include "software_global.h"

#include "Color.h"
#include "Vector3.h"

namespace paysages {
namespace software {

/**
 * @brief Representation of world coordinates projected in a canvas pixel.
 */
class SOFTWARESHARED_EXPORT CanvasFragment
{
public:
    CanvasFragment() = default;
    CanvasFragment(double z, const Vector3 &location, int client=0, bool opaque=true);

    void setColor(const Color &col);

    inline bool getOpaque() const {return opaque;}
    inline double getZ() const {return z;}
    inline const Vector3 &getLocation() const {return location;}
    inline int getClient() const {return client;}
    inline const Color &getColor() const {return color;}

private:
    bool opaque;
    double z;
    Vector3 location;
    int client;
    Color color;
};

}
}

#endif // CANVASFRAGMENT_H
