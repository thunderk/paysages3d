#ifndef DEFINITION_GLOBAL_H
#define DEFINITION_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DEFINITION_LIBRARY)
#  define DEFINITIONSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DEFINITIONSHARED_EXPORT Q_DECL_IMPORT
#endif

#include "basics_global.h"

namespace paysages {
namespace definition {
    class BaseDefinition;
    class Scenery;
    class CameraDefinition;
    class SurfaceMaterial;
    class Zone;
    class WaterDefinition;
    class Layers;
    class CloudsDefinition;
    class CloudLayerDefinition;
    class AtmosphereDefinition;
    class TexturesDefinition;
    class TextureLayerDefinition;
    class TerrainDefinition;
    class TerrainHeightMap;
    class TerrainHeightMapBrush;
    class TerrainHeightMapBrushElevation;
    class TerrainHeightMapBrushSmooth;
    class TerrainHeightMapBrushAddNoise;
    class TerrainHeightMapBrushReset;
    class TerrainHeightMapBrushFlatten;
}
}
using namespace paysages::definition;

#endif // DEFINITION_GLOBAL_H
