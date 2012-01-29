#include "renderer.h"
#include "shared/constants.h"
#include "lighting.h"

RayCastingResult _RAYCASTING_NULL = {0};
HeightInfo _WATER_HEIGHT_INFO = {-1000000.0, -1000000.0, -1000000.0};

static Color _filterLight(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light)
{
    return light_color;
}

static Color _maskLight(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light)
{
    return light_color;
}

static Color _applyLightingToSurface(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material)
{
    return material.base;
}

static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds)
{
    return _RAYCASTING_NULL;
}

static double _getTerrainHeight(Renderer* renderer, double x, double z)
{
    return 0.0;
}

static HeightInfo _getWaterHeightInfo(Renderer* renderer)
{
    return _WATER_HEIGHT_INFO;
}

static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
{
    return COLOR_TRANSPARENT;
}

static Color _applyAtmosphere(Renderer* renderer, Vector3 location, Color base)
{
    return base;
}

static Color _applyClouds(Renderer* renderer, Color base, Vector3 start, Vector3 end)
{
    return base;
}

static Vector3 _projectPoint(Renderer* renderer, Vector3 point)
{
    return point;
}

static Vector3 _unprojectPoint(Renderer* renderer, Vector3 point)
{
    return point;
}

static double _getPrecision(Renderer* renderer, Vector3 location)
{
    return 0.001;
}

Renderer rendererGetFake()
{
    Renderer result;

    result.render_quality = 5;
    result.camera_location = VECTOR_ZERO;

    result.filterLight = _filterLight;
    result.maskLight = _maskLight;
    result.applyLightingToSurface = _applyLightingToSurface;
    result.rayWalking = _rayWalking;
    result.getTerrainHeight = _getTerrainHeight;
    result.getWaterHeightInfo = _getWaterHeightInfo;
    result.applyTextures = _applyTextures;
    result.applyAtmosphere = _applyAtmosphere;
    result.applyClouds = _applyClouds;
    result.projectPoint = _projectPoint;
    result.unprojectPoint = _unprojectPoint;
    result.getPrecision = _getPrecision;

    return result;
}
