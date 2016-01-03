#ifndef CANVASFRAGMENT_H
#define CANVASFRAGMENT_H

#include "software_global.h"

#include "Color.h"
#include "Vector3.h"

namespace paysages {
namespace software {

/**
 * Representation of world coordinates projected in a canvas pixel.
 */
class SOFTWARESHARED_EXPORT CanvasFragment {
  public:
    CanvasFragment() = default;
    CanvasFragment(bool front_facing, const Vector3 &pixel, const Vector3 &location, int client = 0,
                   bool opaque = true);

    void setColor(const Color &col);

    inline bool getOpaque() const {
        return opaque;
    }
    inline bool isFrontFacing() const {
        return front_facing;
    }
    inline double getZ() const {
        return pixel.z;
    }
    inline const Vector3 &getLocation() const {
        return location;
    }
    inline const Vector3 &getPixel() const {
        return pixel;
    }
    inline int getClient() const {
        return client;
    }
    inline const Color &getColor() const {
        return color;
    }

  private:
    bool opaque;
    bool front_facing;
    Vector3 pixel;
    Vector3 location;
    int client;
    Color color;
};
}
}

#endif // CANVASFRAGMENT_H
