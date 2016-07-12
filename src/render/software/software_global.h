#pragma once

#include "definition_global.h"

#if defined(SOFTWARE_LIBRARY)
#define SOFTWARESHARED_EXPORT DECL_EXPORT
#else
#define SOFTWARESHARED_EXPORT DECL_IMPORT
#endif

namespace paysages {
namespace software {
class SoftwareRenderer;
class SoftwareCanvasRenderer;
class RenderConfig;
class RenderProgress;

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

class Rasterizer;
class OverlayRasterizer;
class SkyRasterizer;
class TerrainRasterizer;
class VegetationRasterizer;

class LightingManager;
class LightStatus;
class LightFilter;
class LightComponent;
class LightSource;

class RayCastingManager;
class RayCastingResult;

class NightSky;
class MoonRenderer;

class TerrainRayWalker;

class GodRaysSampler;
class GodRaysResult;
class VegetationResult;
class VegetationRenderer;
class VegetationModelRenderer;

class Canvas;
class CanvasPortion;
class CanvasPixel;
class CanvasFragment;
class CanvasLiveClient;
class CanvasPreview;
class CanvasPixelShader;
class CanvasPictureWriter;
}
}

using namespace paysages::software;
