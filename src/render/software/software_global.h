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
    class AtmosphereResult;
    class AtmosphereModelBruneton;

    class CloudsRenderer;
    class BaseCloudLayerRenderer;
    class BaseCloudsModel;

    class TerrainRenderer;
    class TexturesRenderer;
    class WaterRenderer;

    class SkyRasterizer;
    class TerrainRasterizer;

    class LightingManager;
    class LightStatus;
    class LightFilter;
    class LightComponent;
}
}

using namespace paysages::software;

#endif // SOFTWARE_GLOBAL_H
