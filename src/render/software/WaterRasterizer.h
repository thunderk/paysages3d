#ifndef WATERRASTERIZER_H
#define WATERRASTERIZER_H

#include "software_global.h"

#include "Rasterizer.h"

namespace paysages {
namespace software {

class WaterRasterizer: public Rasterizer
{
public:
    WaterRasterizer(SoftwareRenderer* renderer, int client_id);

    virtual void rasterize();
};

}
}

#endif // WATERRASTERIZER_H
