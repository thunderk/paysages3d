#pragma once

#include "basics_global.h"

#if defined(DEFINITION_LIBRARY)
#define DEFINITIONSHARED_EXPORT DECL_EXPORT
#else
#define DEFINITIONSHARED_EXPORT DECL_IMPORT
#endif

namespace paysages {
namespace definition {
class DefinitionNode;
class DefinitionDiff;
class FloatNode;
class FloatDiff;
class IntNode;
class IntDiff;
class NoiseNode;
class ColorNode;
class ColorDiff;
class DiffManager;
class DefinitionWatcher;
class Scenery;
class CameraDefinition;
class SurfaceMaterial;
class MaterialNode;
class Zone;
class WaterDefinition;
class Layers;
class LayersDiff;
class CloudsDefinition;
class CloudLayerDefinition;
class AtmosphereDefinition;
class CelestialBodyDefinition;
class GodRaysDefinition;
class TexturesDefinition;
class TextureLayerDefinition;
class TerrainDefinition;
class TerrainHeightMap;
class VegetationDefinition;
class VegetationLayerDefinition;
class VegetationModelDefinition;
class VegetationPresenceDefinition;
class VegetationInstance;
class PaintedGrid;
class PaintedGridData;
class PaintedGridBrush;
class PaintedGridBrushRaiseLower;
class PaintedGridBrushSmooth;
class PaintedGridBrushAddNoise;
class PaintedGridBrushReset;
class PaintedGridBrushFlatten;
class TimeManager;
}
}
using namespace paysages::definition;
