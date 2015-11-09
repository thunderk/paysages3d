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
#include "Rasterizer.h"
#include "SpaceSegment.h"
#include "OverlayRasterizer.h"
#include "VegetationModelDefinition.h"
#include "VegetationInstance.h"
#include "VegetationRenderer.h"
#include "RayCastingResult.h"

#include <sstream>

void startRender(SoftwareCanvasRenderer *renderer, const char *outputpath);

static void startTestRender(SoftwareCanvasRenderer *renderer, const std::string &name, int iteration = -1) {
    std::ostringstream stream;

    stream << "pic_test_" << name;
    if (iteration >= 0) {
        stream << "_";
        stream.width(4);
        stream.fill('0');
        stream << iteration;
    }
    stream << ".png";

    startRender(renderer, stream.str().data());
}

static void testGroundShadowQuality() {
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
    for (int i = 0; i < 6; i++) {
        // TODO keep same rasterization across renders
        renderer.getTerrainRenderer()->setQuality((double)i / 5.0);
        startTestRender(&renderer, "ground_shadow_quality", i);
    }
}

static void testRasterizationQuality() {
    Scenery scenery;
    scenery.autoPreset(12);

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(800, 600);
    renderer.enablePostprocess(false);
    for (int i = 0; i < 6; i++) {
        renderer.setQuality((double)i / 5.0);
        startTestRender(&renderer, "rasterization_quality", i);
    }
}

static void testCloudQuality() {
    Scenery scenery;
    scenery.autoPreset(3);
    scenery.getCamera()->setLocation(Vector3(5.0, 5.0, 5.0));
    scenery.getCamera()->setTarget(Vector3(8.0, 7.25, 8.0));
    scenery.getTerrain()->height = 0.0;
    scenery.getTerrain()->validate();

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(600, 800);
    SkyRasterizer rasterizer(&renderer, renderer.getProgressHelper(), 0);
    renderer.setSoloRasterizer(&rasterizer);
    for (int i = 0; i < 6; i++) {
        renderer.setQuality((double)i / 5.0);
        rasterizer.setQuality(0.2);
        startTestRender(&renderer, "cloud_quality", i);
    }
}

static void testGodRays() {
    class TestLightFilter : public LightFilter {
        virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override {
            if (Vector3(0.0, 100.0, 0.0).sub(at).normalize().y > 0.97) {
                light.color = COLOR_BLACK;
                return false;
            } else {
                return true;
            }
        }
    };
    class TestRenderer : public SoftwareCanvasRenderer {
      public:
        TestRenderer(Scenery *scenery) : SoftwareCanvasRenderer(scenery) {
        }

      private:
        virtual void prepare() override {
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
    SkyRasterizer rasterizer(&renderer, renderer.getProgressHelper(), 0);
    renderer.setSoloRasterizer(&rasterizer);
    TestLightFilter filter;
    renderer.getLightingManager()->clearFilters();
    renderer.getLightingManager()->registerFilter(&filter);

    // quality
    for (int i = 0; i < 6; i++) {
        renderer.setQuality((double)i / 5.0);
        rasterizer.setQuality(0.2);
        startTestRender(&renderer, "god_rays_quality", i);
    }
    renderer.setQuality(0.5);

    // penetration
    for (int i = 0; i < 3; i++) {
        scenery.getAtmosphere()->childGodRays()->propPenetration()->setValue(0.01 + 0.02 * (double)i);
        startTestRender(&renderer, "god_rays_penetration", i);
    }

    // resistance
    scenery.getAtmosphere()->childGodRays()->propPenetration()->setValue(0.01);
    for (int i = 0; i < 3; i++) {
        scenery.getAtmosphere()->childGodRays()->propResistance()->setValue(0.1 + 0.1 * (double)i);
        startTestRender(&renderer, "god_rays_resistance", i);
    }

    // boost
    scenery.getAtmosphere()->childGodRays()->propResistance()->setValue(0.3);
    for (int i = 0; i < 3; i++) {
        scenery.getAtmosphere()->childGodRays()->propBoost()->setValue(2.0 + 4.0 * (double)i);
        startTestRender(&renderer, "god_rays_boost", i);
    }
}

static void testNearFrustum() {
    Scenery scenery;
    scenery.autoPreset(3);
    scenery.getCamera()->setLocation(Vector3(0.0, 0.0, 0.0));
    scenery.getCamera()->setTarget(Vector3(1.0, 0.0, 1.0));

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(400, 300);
    renderer.setQuality(0.1);
    startTestRender(&renderer, "near_frustum_good");

    renderer.getWaterRasterizer()->setAutoCutLimit(1000.0);
    startTestRender(&renderer, "near_frustum_bad");
}

static void testCloudsNearGround() {
    Scenery scenery;
    scenery.autoPreset(8);
    scenery.getAtmosphere()->setDayTime(6, 20);

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(400, 300);
    renderer.setQuality(0.3);

    startTestRender(&renderer, "clouds_near_ground", 1);

    scenery.getCamera()->strafeUp(6.0);
    scenery.getCamera()->copy(renderer.render_camera);
    startTestRender(&renderer, "clouds_near_ground", 2);
}

static void testSunNearHorizon() {
    Scenery scenery;
    scenery.autoPreset(28);
    scenery.getCamera()->setLocation(VECTOR_ZERO);
    scenery.getCamera()->setTarget(VECTOR_EAST);
    scenery.getClouds()->clear();
    scenery.getTextures()->applyPreset(TexturesDefinition::TEXTURES_PRESET_CANYON);
    scenery.getTerrain()->propWaterHeight()->setValue(-1.0);

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(400, 300);
    renderer.setQuality(0.3);

    for (int i = 0; i <= 20; i++) {
        scenery.getAtmosphere()->propDayTime()->setValue(0.24 + 0.001 * (double)i);
        startTestRender(&renderer, "sun_near_horizon", i);
    }
}

static void testVegetationModels() {
    class InstanceRenderer : public SoftwareCanvasRenderer, public OverlayRasterizer, public LightFilter {
      public:
        InstanceRenderer(Scenery *scenery, const VegetationModelDefinition &model)
            : SoftwareCanvasRenderer(scenery), OverlayRasterizer(this, this->getProgressHelper()),
              instance(model, VECTOR_ZERO), vegetation(renderer->getVegetationRenderer()) {
        }
        virtual void prepare() override {
            SoftwareCanvasRenderer::prepare();

            getLightingManager()->clearFilters();
            getLightingManager()->registerFilter(this);
            // TODO Add filter for vegetation instance (for self shadows)
        }
        virtual Color applyMediumTraversal(const Vector3 &, const Color &color) override {
            return color;
        }
        virtual Color processPixel(int, int, double relx, double rely) const override {
            relx *= 0.75;
            rely *= 0.75;
            SpaceSegment segment(Vector3(relx, rely + 0.5, 5.0), Vector3(relx, rely + 0.5, -5.0));
            RayCastingResult result = vegetation->renderInstance(segment, instance, false, true);
            return result.hit ? result.hit_color : Color(0.6, 0.7, 0.9);
        }
        virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override {
            SpaceSegment segment(at, at.add(light.direction.scale(-5.0)));
            RayCastingResult result = vegetation->renderInstance(segment, instance, true, true);
            return not result.hit;
        }
        VegetationInstance instance;
        VegetationRenderer *vegetation;
    };

    Scenery scenery;
    scenery.autoPreset(1);
    scenery.getClouds()->clear();
    scenery.getCamera()->setTarget(VECTOR_ZERO);
    scenery.getCamera()->setLocation(Vector3(0.0, 0.0, 5.0));
    int width = 800;
    int height = 800;

    for (int i = 0; i < 10; i++) {
        // TODO Make random sequence repeatable
        VegetationModelDefinition model(NULL);
        InstanceRenderer renderer(&scenery, model);
        renderer.setSize(width, height);
        renderer.setSoloRasterizer(&renderer);

        startTestRender(&renderer, "vegetation_model_basic", i);
    }
}

void runTestSuite() {
    testGroundShadowQuality();
    testRasterizationQuality();
    testCloudQuality();
    testGodRays();
    testNearFrustum();
    testCloudsNearGround();
    testSunNearHorizon();
    testVegetationModels();
}
