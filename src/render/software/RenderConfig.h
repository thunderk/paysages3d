#pragma once

#include "software_global.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT RenderConfig {
  public:
    RenderConfig(int width = 0, int height = 0, int antialias = 1, int quality = 5);

    int width;
    int height;
    int antialias;
    int quality;
};
}
}
