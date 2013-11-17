#include "SoftwareRenderer.h"

#include "CameraDefinition.h"
#include "Scenery.h"
#include "FluidMediumManager.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereDefinition.h"


// Legacy compatibility
#include "renderer.h"
#include "terrain/public.h"
#include "clouds/public.h"
#include "textures/public.h"
#include "water/public.h"
static AtmosphereResult _legacyApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base)
{
    return ((SoftwareRenderer*)renderer)->getAtmosphereRenderer()->applyAerialPerspective(location, base);
}
static AtmosphereResult _legacyGetSkyColor(Renderer* renderer, Vector3 direction)
{
    return ((SoftwareRenderer*)renderer)->getAtmosphereRenderer()->getSkyColor(direction);
}
static void _legacyGetLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque)
{
    return ((SoftwareRenderer*)renderer)->getAtmosphereRenderer()->getLightingStatus(status, normal, opaque);
}
static Vector3 _legacyGetSunDirection(Renderer* renderer)
{
    return ((SoftwareRenderer*)renderer)->getAtmosphereRenderer()->getSunDirection();
}
static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int, int, int, int)
{
    RayCastingResult result;
    Color sky_color;

    result = renderer->terrain->castRay(renderer, location, direction);
    if (!result.hit)
    {
        sky_color = renderer->atmosphere->getSkyColor(renderer, direction).final;

        result.hit = 1;
        result.hit_location = v3Add(location, v3Scale(direction, 1000.0));
        result.hit_color = renderer->clouds->getColor(renderer, sky_color, location, result.hit_location);
    }

    return result;
}
static double _getPrecision(Renderer* renderer, Vector3 location)
{
    Vector3 projected;

    projected = renderer->render_camera->project(location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return v3Norm(v3Sub(renderer->render_camera->unproject(projected), location)); // / (double)render_quality;
}

SoftwareRenderer::SoftwareRenderer(Scenery* scenery)
{
    atmosphere_renderer = new BaseAtmosphereRenderer(this);

    fluid_medium = new FluidMediumManager(this);

    if (scenery)
    {
        this->scenery = scenery;
        own_scenery = false;
    }
    else
    {
        this->scenery = new Scenery;
        own_scenery = true;
    }
}

SoftwareRenderer::~SoftwareRenderer()
{
    delete atmosphere_renderer;

    delete fluid_medium;

    if (own_scenery)
    {
        delete scenery;
    }
}

void SoftwareRenderer::setScenery(Scenery* scenery)
{
    if (!own_scenery)
    {
        this->scenery = new Scenery;
        own_scenery = true;
    }
    scenery->copy(this->scenery);
}

void SoftwareRenderer::prepare()
{
    // Prepare sub renderers
    delete atmosphere_renderer;
    atmosphere_renderer = new SoftwareBrunetonAtmosphereRenderer(this);

    // Setup transitional renderers (for C-legacy subsystems)
    rayWalking = _rayWalking;
    getPrecision = _getPrecision;

    scenery->getAtmosphere()->copy(atmosphere->definition);
    atmosphere->applyAerialPerspective = _legacyApplyAerialPerspective;
    atmosphere->getSkyColor = _legacyGetSkyColor;
    atmosphere->getLightingStatus = _legacyGetLightingStatus;
    atmosphere->getSunDirection = _legacyGetSunDirection;

    scenery->getCamera()->copy(render_camera);
    TerrainRendererClass.bind(this, scenery->getTerrain());
    TexturesRendererClass.bind(this, scenery->getTextures());
    CloudsRendererClass.bind(this, scenery->getClouds());
    WaterRendererClass.bind(this, scenery->getWater());

    // Prepare global tools
    fluid_medium->clearMedia();
    //fluid_medium->registerMedium(water_renderer);
}

/*Color SoftwareRenderer::applyMediumTraversal(Vector3 location, Color color)
{
    Vector3 eye = cameraGetLocation(scenery->getCamera());
    return fluid_medium->applyTraversal(eye, location, color);
}*/
