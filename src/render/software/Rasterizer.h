#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * @brief Base abstract class for scenery pieces that can be rasterized to polygons.
 */
class SOFTWARESHARED_EXPORT Rasterizer
{
public:
    Rasterizer();
};

}
}

#endif // RASTERIZER_H
