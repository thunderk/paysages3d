#include "SoftwareCanvasRenderer.h"
#include "TerrainRenderer.h"
#include "Scenery.h"
#include "CameraDefinition.h"
#include "TerrainDefinition.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
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
#include "OpenGLVegetationImpostor.h"
#include "Texture2D.h"
#include "NoiseNode.h"
#include "FractalNoise.h"
#include "RandomGenerator.h"
#include "NoiseFunctionSimplex.h"

#include <sstream>
#include <iostream>

void startRender(SoftwareCanvasRenderer *renderer, const char *outputpath);

static string getFileName(const string &name, int iteration = -1) {
    ostringstream stream;

    stream << "pic_test_" << name;
    if (iteration >= 0) {
        stream << "_";
        stream.width(4);
        stream.fill('0');
        stream << iteration;
    }
    stream << ".png";

    return stream.str();
}

static void startTestRender(SoftwareCanvasRenderer *renderer, const string &name, int iteration = -1) {
    startRender(renderer, getFileName(name, iteration).data());
}

static void testNoise() {
    NoiseFunctionSimplex noise;

    string filename = getFileName("noise_simplex_cache_value");
    cout << "Rendering " << filename << "..." << endl;
    noise.getValueTexture()->saveToFile(filename);

    filename = getFileName("noise_simplex_cache_normal");
    cout << "Rendering " << filename << "..." << endl;
    noise.getNormalTexture()->saveToFile(filename);
}

static void testGroundShadowQuality() {
    Scenery scenery;
    RandomGenerator random(5);
    scenery.getTerrain()->applyPreset(TerrainDefinition::TERRAIN_PRESET_STANDARD, random);
    scenery.getTerrain()->propWaterHeight()->setValue(-0.5);
    scenery.getWater()->propReflection()->setValue(0.0);
    scenery.getWater()->material->base->r = 0.0;
    scenery.getWater()->material->base->g = 0.3;
    scenery.getWater()->material->base->b = 1.0;
    scenery.getWater()->material->reflection = 0.0;
    scenery.getWater()->foam_coverage = 0.0;
    scenery.getWater()->transparency = 0.0;
    scenery.getAtmosphere()->applyPreset(AtmosphereDefinition::ATMOSPHERE_PRESET_CLEAR_SUNSET, random);
    scenery.getAtmosphere()->setDayTime(16, 45);
    scenery.getTextures()->clear();
    TextureLayerDefinition texture(NULL, "test");
    texture.displacement_height = 0.3;
    texture.displacement_scaling = 2.0;
    texture.material->setColor(0.6, 0.55, 0.57, 1.0);
    texture.material->reflection = 0.006;
    texture.material->shininess = 6.0;
    texture.validate();
    scenery.getTextures()->addLayer(texture);
    scenery.getCamera()->setLocation(Vector3(10.0, 10.0, -10.0));
    scenery.getCamera()->setTarget(VECTOR_ZERO);
    scenery.validate();

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(400, 300);
    renderer.setQuality(0.2);
    for (int i = 0; i < 6; i++) {
        // TODO keep same rasterization across renders
        renderer.getTerrainRenderer()->setQuality(to_double(i) / 5.0);
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
        renderer.setQuality(to_double(i) / 5.0);
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
        renderer.setQuality(to_double(i) / 5.0);
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
        renderer.setQuality(to_double(i) / 5.0);
        rasterizer.setQuality(0.2);
        startTestRender(&renderer, "god_rays_quality", i);
    }
    renderer.setQuality(0.5);

    // penetration
    for (int i = 0; i < 3; i++) {
        scenery.getAtmosphere()->childGodRays()->propPenetration()->setValue(0.01 + 0.02 * to_double(i));
        startTestRender(&renderer, "god_rays_penetration", i);
    }

    // resistance
    scenery.getAtmosphere()->childGodRays()->propPenetration()->setValue(0.01);
    for (int i = 0; i < 3; i++) {
        scenery.getAtmosphere()->childGodRays()->propResistance()->setValue(0.1 + 0.1 * to_double(i));
        startTestRender(&renderer, "god_rays_resistance", i);
    }

    // boost
    scenery.getAtmosphere()->childGodRays()->propResistance()->setValue(0.3);
    for (int i = 0; i < 3; i++) {
        scenery.getAtmosphere()->childGodRays()->propBoost()->setValue(2.0 + 4.0 * to_double(i));
        startTestRender(&renderer, "god_rays_boost", i);
    }
}

static void testNearFrustum() {
    Scenery scenery;
    scenery.autoPreset(3);
    scenery.getClouds()->clear();
    scenery.getCamera()->setLocation(Vector3(0.0, 0.0, 0.0));
    scenery.getCamera()->setTarget(Vector3(1.0, 0.0, 1.0));
    scenery.getTerrain()->propWaterHeight()->setValue(5.0);
    scenery.keepCameraAboveGround(scenery.getCamera());

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(400, 300);
    renderer.setQuality(0.1);

    renderer.enablePostprocess(false);
    startTestRender(&renderer, "near_frustum_good_raw");
    renderer.enablePostprocess(true);
    startTestRender(&renderer, "near_frustum_good");

    renderer.getWaterRasterizer()->setAutoCutLimit(1000.0);
    renderer.enablePostprocess(false);
    startTestRender(&renderer, "near_frustum_bad_raw");
    renderer.enablePostprocess(true);
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

static void testAtmosphereBruneton() {
    Scenery scenery;
    scenery.getAtmosphere()->applyPreset(AtmosphereDefinition::ATMOSPHERE_PRESET_CLEAR_DAY);
    scenery.getCamera()->setTarget(Vector3(10.0, 1.0, 0.0));
    scenery.getCamera()->setLocation(Vector3(0.0, 1.0, 0.0));

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(800, 400);
    renderer.getGodRaysSampler()->setEnabled(false);

    class TestRasterizer : public OverlayRasterizer {
      public:
        TestRasterizer(SoftwareCanvasRenderer *renderer)
            : OverlayRasterizer(renderer, renderer->getProgressHelper()), renderer(renderer) {
        }
        virtual Color processPixel(int, int, double relx, double rely) const override {
            if (rely > 0.0) {
                Vector3 dir = (relx < 0.0) ? Vector3(1.0, rely, relx + 1.0) : Vector3(-1.0, rely, relx - 1.0);
                auto result = renderer->getAtmosphereRenderer()->getSkyColor(dir);
                return result.final;
            } else {
                auto result = renderer->getAtmosphereRenderer()->applyAerialPerspective(
                    Vector3(-relx * 400.0, (rely + 1.0) * 20.0, 0.0), COLOR_BLACK);
                return result.final;
            }
        }
        SoftwareCanvasRenderer *renderer;
    };
    TestRasterizer rasterizer(&renderer);
    renderer.setSoloRasterizer(&rasterizer);

    for (int i = 0; i <= 60; i++) {
        double daytime = (i < 40) ? (0.24 + 0.0005 * to_double(i)) : (0.26 + 0.005 * to_double(i - 40));
        scenery.getAtmosphere()->setDayTime(daytime);
        startTestRender(&renderer, "atmosphere_bruneton", i);
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

static void testOpenGLVegetationImpostor() {
    for (int i = 0; i < 4; i++) {
        string filename = getFileName("opengl_vegetation_impostor", i);
        cout << "Rendering " << filename << "..." << endl;

        Scenery scenery;
        scenery.autoPreset(i);

        OpenGLVegetationImpostor impostor(128);
        VegetationModelDefinition model(NULL);

        bool interrupted = false;
        impostor.prepareTexture(model, scenery, &interrupted);

        impostor.getTexture()->saveToFile(filename);
    }
}

void runTestSuite() {
    testAtmosphereBruneton();
    testCloudQuality();
    testCloudsNearGround();
    testGodRays();
    testGroundShadowQuality();
    testNearFrustum();
    testNoise();
    testOpenGLVegetationImpostor();
    testRasterizationQuality();
    testVegetationModels();
}
