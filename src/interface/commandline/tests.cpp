#include "SoftwareCanvasRenderer.h"
#include "TerrainRenderer.h"
#include "Scenery.h"
#include "CameraDefinition.h"
#include "TerrainDefinition.h"
#include "AtmosphereDefinition.h"
#include "TexturesDefinition.h"
#include "TextureLayerDefinition.h"
#include "WaterDefinition.h"
#include "SurfaceMaterial.h"
#include "FloatNode.h"

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
        // TODO keep same rasterization across renders, or keep rasterization quality low
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

void runTestSuite()
{
    testGroundShadowQuality();
    testRasterizationQuality();
}

