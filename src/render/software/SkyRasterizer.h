#ifndef SKYRASTERIZER_H
#define SKYRASTERIZER_H

#include "software_global.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT SkyRasterizer
{
public:
    SkyRasterizer(SoftwareRenderer* renderer);
    void rasterize();

private:
    SoftwareRenderer* renderer;
};

}
}

#endif // SKYRASTERIZER_H
