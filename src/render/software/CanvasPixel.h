#ifndef CANVASPIXEL_H
#define CANVASPIXEL_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * @brief One pixel of a Canvas.
 *
 * A pixel stores superimposed fragments (CanvasFragment), sorted by their distance to camera.
 */
class SOFTWARESHARED_EXPORT CanvasPixel
{
public:
    CanvasPixel();
};

}
}

#endif // CANVASPIXEL_H
