#ifndef SOFTWARE_GLOBAL_H
#define SOFTWARE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SOFTWARE_LIBRARY)
#  define SOFTWARESHARED_EXPORT Q_DECL_EXPORT
#else
#  define SOFTWARESHARED_EXPORT Q_DECL_IMPORT
#endif

#include "definition_global.h"

namespace paysages {
namespace software {
    class SoftwareRenderer;

    class FluidMediumManager;
    class FluidMediumInterface;
    class FluidMediumCollector;

    class BaseAtmosphereRenderer;
    class SoftwareBrunetonAtmosphereRenderer;

    class CloudsRenderer;
    class BaseCloudLayerRenderer;
    class BaseCloudsModel;

    class SkyRasterizer;
    class TerrainRasterizer;
}
}

using namespace paysages::software;

#endif // SOFTWARE_GLOBAL_H
