#include "SoftwareCanvasRenderer.h"
#include "TerrainRenderer.h"
#include "Scenery.h"
#include "CameraDefinition.h"
#include "TerrainDefinition.h"
#include "AtmosphereDefinition.h"
#include "TexturesDefinition.h"
#include "GodRaysDefinition.h"
#include "TextureLayerDefinition.h"
#include "WaterDefinition.h"
#include "SurfaceMaterial.h"
#include "FloatNode.h"
#include "SkyRasterizer.h"
#include "CloudsDefinition.h"
#include "LightComponent.h"
#include "LightingManager.h"
#include "LightFilter.h"
#include "GodRaysSampler.h"

#include <sstream>

void startRender(SoftwareCanvasRenderer *renderer, const char *outputpath);

static void startTestRender(SoftwareCanvasRenderer *renderer, const std::string &name, int iteration)
{
    std::ostringstream stream;

    stream << "pic_test_" << name << "_";
    stream.width(4);
    stream.fill('0');
    stream << iteration;
    stream << ".png";

    startRender(renderer, stream.str().data());
}

static void testGroundShadowQuality()
{
    Scenery scenery;
    srand(5);
    scenery.getTerrain()->applyPreset(TerrainDefinition::TERRAIN_PRESET_STANDARD);
    scenery.getTerrain()->propWaterHeight()->setValue(-0.5);
    scenery.getWater()->propReflection()->setValue(0.0);
    scenery.getWater()->material->base->r = 0.0;
    scenery.getWater()->material->base->g = 0.3;
    scenery.getWater()->material->base->b = 1.0;
    scenery.getWater()->material->reflection = 0.0;
    scenery.getWater()->foam_coverage = 0.0;
    scenery.getWater()->transparency = 0.0;
    scenery.getAtmosphere()->applyPreset(AtmosphereDefinition::ATMOSPHERE_PRESET_CLEAR_SUNSET);
    scenery.getAtmosphere()->setDayTime(16, 45);
    scenery.getTextures()->clear();
    TextureLayerDefinition *texture = scenery.getTextures()->getTextureLayer(scenery.getTextures()->addLayer());
    texture->displacement_height = 0.3;
    texture->displacement_scaling = 2.0;
    texture->displacement_offset = 0.0;
    texture->material->setColor(0.6, 0.55, 0.57, 1.0);
    texture->material->reflection = 0.006;
    texture->material->shininess = 6.0;
    texture->validate();
    scenery.getCamera()->setLocation(Vector3(10.0, 10.0, -10.0));
    scenery.getCamera()->setTarget(VECTOR_ZERO);
    scenery.validate();

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(400, 300);
    renderer.setQuality(0.2);
    for (int i = 0; i < 6; i++)
    {
        // TODO keep same rasterization across renders
        renderer.getTerrainRenderer()->setQuality((double)i / 5.0);
        startTestRender(&renderer, "ground_shadow_quality", i);
    }
}

static void testRasterizationQuality()
{
    Scenery scenery;
    scenery.autoPreset(12);

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(800, 600);
    renderer.enablePostprocess(false);
    for (int i = 0; i < 6; i++)
    {
        renderer.setQuality((double)i / 5.0);
        startTestRender(&renderer, "rasterization_quality", i);
    }
}

static void testCloudQuality()
{
    Scenery scenery;
    scenery.autoPreset(3);
    scenery.getCamera()->setLocation(Vector3(5.0, 5.0, 5.0));
    scenery.getCamera()->setTarget(Vector3(8.0, 7.25, 8.0));
    scenery.getTerrain()->height = 0.0;
    scenery.getTerrain()->validate();

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(600, 800);
    SkyRasterizer *rasterizer = new SkyRasterizer(&renderer, renderer.getProgressHelper(), 0);
    renderer.setSoloRasterizer(rasterizer);
    for (int i = 0; i < 6; i++)
    {
        renderer.setQuality((double)i / 5.0);
        rasterizer->setQuality(0.2);
        startTestRender(&renderer, "cloud_quality", i);
    }
}

static void testGodRays()
{
    class TestLightFilter: public LightFilter
    {
        virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override
        {
            if (Vector3(0.0, 100.0, 0.0).sub(at).normalize().y > 0.97)
            {
                light.color = COLOR_BLACK;
                return false;
            }
            else
            {
                return true;
            }
        }
    };
    class TestRenderer: public SoftwareCanvasRenderer
    {
    public:
        TestRenderer(Scenery *scenery): SoftwareCanvasRenderer(scenery) {}
    private:
        virtual void prepare() override
        {
            SoftwareRenderer::prepare();

            getLightingManager()->clearSources();
            getLightingManager()->addStaticLight(LightComponent(COLOR_WHITE, VECTOR_DOWN));
            getGodRaysSampler()->setAltitudes(0.0, 30.0);
            getGodRaysSampler()->reset();
        }
    };

    Scenery scenery;
    scenery.autoPreset(63);
    scenery.getAtmosphere()->setDayTime(12);
    scenery.getCamera()->setLocation(Vector3(0.0, 1.0, -50.0));
    scenery.getCamera()->setTarget(Vector3(0.0, 15.0, 0.0));
    scenery.getTerrain()->height = 0.0;
    scenery.getTerrain()->validate();
    scenery.getClouds()->clear();

    TestRenderer renderer(&scenery);
    renderer.setSize(500, 300);
    SkyRasterizer *rasterizer = new SkyRasterizer(&renderer, renderer.getProgressHelper(), 0);
    renderer.setSoloRasterizer(rasterizer);
    TestLightFilter filter;
    renderer.getLightingManager()->clearFilters();
    renderer.getLightingManager()->registerFilter(&filter);

    // quality
    for (int i = 0; i < 6; i++)
    {
        renderer.setQuality((double)i / 5.0);
        rasterizer->setQuality(0.2);
        startTestRender(&renderer, "god_rays_quality", i);
    }
    renderer.setQuality(0.5);

    // penetration
    for (int i = 0; i < 3; i++)
    {
        scenery.getAtmosphere()->childGodRays()->propPenetration()->setValue(0.01 + 0.02 * (double)i);
        startTestRender(&renderer, "god_rays_penetration", i);
    }

    // resistance
    scenery.getAtmosphere()->childGodRays()->propPenetration()->setValue(0.01);
    for (int i = 0; i < 3; i++)
    {
        scenery.getAtmosphere()->childGodRays()->propResistance()->setValue(0.1 + 0.1 * (double)i);
        startTestRender(&renderer, "god_rays_resistance", i);
    }

    // boost
    scenery.getAtmosphere()->childGodRays()->propResistance()->setValue(0.3);
    for (int i = 0; i < 3; i++)
    {
        scenery.getAtmosphere()->childGodRays()->propBoost()->setValue(2.0 + 4.0 * (double)i);
        startTestRender(&renderer, "god_rays_boost", i);
    }
}

void runTestSuite()
{
    testGroundShadowQuality();
    testRasterizationQuality();
    testCloudQuality();
    testGodRays();
}

