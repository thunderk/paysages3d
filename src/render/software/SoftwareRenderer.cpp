#include "SoftwareRenderer.h"

#include "CameraDefinition.h"
#include "Scenery.h"
#include "FluidMediumManager.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereDefinition.h"


// Legacy compatibility
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
    this->scenery->bindToRenderer(this);
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

void SoftwareRenderer::prepare()
{
    // Prepare sub renderers
    delete atmosphere_renderer;
    atmosphere_renderer = new SoftwareBrunetonAtmosphereRenderer(this);

    // Setup transitional renderers (for C-legacy subsystems)
    scenery->getAtmosphere()->copy(atmosphere->definition);
    atmosphere->applyAerialPerspective = _legacyApplyAerialPerspective;
    atmosphere->getSkyColor = _legacyGetSkyColor;
    atmosphere->getLightingStatus = _legacyGetLightingStatus;
    atmosphere->getSunDirection = _legacyGetSunDirection;

    // Prepare global tools
    fluid_medium->clearMedia();
    //fluid_medium->registerMedium(water_renderer);
}

/*Color SoftwareRenderer::applyMediumTraversal(Vector3 location, Color color)
{
    Vector3 eye = cameraGetLocation(scenery->getCamera());
    return fluid_medium->applyTraversal(eye, location, color);
}*/
