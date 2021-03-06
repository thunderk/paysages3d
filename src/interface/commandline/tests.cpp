#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "CameraDefinition.h"
#include "CelestialBodyDefinition.h"
#include "CloudLayerDefinition.h"
#include "CloudsDefinition.h"
#include "CloudsRenderer.h"
#include "FloatNode.h"
#include "FractalNoise.h"
#include "GodRaysDefinition.h"
#include "GodRaysSampler.h"
#include "LightComponent.h"
#include "LightFilter.h"
#include "LightingManager.h"
#include "Maths.h"
#include "NoiseFunctionSimplex.h"
#include "NoiseNode.h"
#include "OpenGLVegetationImpostor.h"
#include "OverlayRasterizer.h"
#include "RandomGenerator.h"
#include "Rasterizer.h"
#include "RayCastingResult.h"
#include "Scenery.h"
#include "SkyRasterizer.h"
#include "SoftwareCanvasRenderer.h"
#include "SpaceSegment.h"
#include "SurfaceMaterial.h"
#include "TerrainDefinition.h"
#include "TerrainRenderer.h"
#include "Texture2D.h"
#include "TextureLayerDefinition.h"
#include "TexturesDefinition.h"
#include "VegetationInstance.h"
#include "VegetationModelDefinition.h"
#include "VegetationRenderer.h"
#include "WaterDefinition.h"
#include "Zone.h"
#include "clouds/BaseCloudsModel.h"

#include <cmath>
#include <iostream>
#include <sstream>

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
    // TODO Test all noise functions similarly

    NoiseFunctionSimplex noise;

    cout << "Testing simplex distribution..." << endl;
    auto report = noise.checkDistribution();
    cout << report;

    string filename = getFileName("noise_simplex_cache_value");
    cout << "Rendering " << filename << "..." << endl;
    noise.getValueTexture()->saveToFile(filename);

    filename = getFileName("noise_simplex_cache_normal");
    cout << "Rendering " << filename << "..." << endl;
    noise.getNormalTexture()->saveToFile(filename);

    const int size = 1024;
    const double scale = 0.01;
    Texture2D pic(size, size);
    for (int i = 0; i < 10; i++) {
        filename = getFileName("noise_simplex_compo", i);
        cout << "Rendering " << filename << "..." << endl;
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                double val =
                    noise.get2d(pow(0.5, to_double(i)), to_double(x) * scale, to_double(y) * scale) * 0.5 + 0.5;
                pic.setPixel(x, y, Color(val, val, val));
            }
        }
        pic.saveToFile(filename);
    }
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
    texture.applyPreset(TextureLayerDefinition::TEXTURES_LAYER_PRESET_ROCK);
    texture.terrain_zone->clear();
    scenery.getTextures()->addLayer(texture);
    scenery.getCamera()->setLocation(Vector3(10.0, 10.0, -10.0));
    scenery.getCamera()->setTarget(VECTOR_ZERO);

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
    scenery.getTerrain()->propHeightNoise()->setConfig(0.0);
    scenery.getTerrain()->validate();

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(600, 800);
    renderer.getGodRaysSampler()->setEnabled(false);
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
    scenery.getTerrain()->propHeightNoise()->setConfig(0.0);
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
        TestRasterizer(SoftwareCanvasRenderer *renderer) : OverlayRasterizer(renderer), renderer(renderer) {
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
            : SoftwareCanvasRenderer(scenery), OverlayRasterizer(this), instance(model, VECTOR_ZERO),
              vegetation(renderer->getVegetationRenderer()) {
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

static void testTextures() {
    Scenery scenery;
    scenery.autoPreset(162);
    scenery.getAtmosphere()->setDayTime(10);
    scenery.getClouds()->clear();
    scenery.getTerrain()->propWaterHeight()->setValue(-1.0);
    scenery.getCamera()->setLocationCoords(0.0, 20.0, 0.0);
    scenery.getCamera()->setTargetCoords(0.0, 0.0, -20.0);

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(600, 600);
    renderer.setQuality(0.5);

    // TODO Customize terrain function

    for (int i = 0; i < 5; i++) {
        TextureLayerDefinition layer(NULL, "test");
        layer.applyPreset(static_cast<TextureLayerDefinition::TextureLayerPreset>(i));
        layer.terrain_zone->clear();
        scenery.getTextures()->clear();
        scenery.getTextures()->addLayer(layer);
        startTestRender(&renderer, "texture", i * 2);

        layer.propDetailNoise()->setConfig(0.0);
        scenery.getTextures()->clear();
        scenery.getTextures()->addLayer(layer);
        startTestRender(&renderer, "texture", i * 2 + 1);
    }

    for (int i = 0; i < 4; i++) {
        scenery.getTextures()->applyPreset(static_cast<TexturesDefinition::TexturesPreset>(i));
        startTestRender(&renderer, "texture_comp", i);
    }
}

static void testCelestialBodies() {
    Scenery scenery;
    scenery.autoPreset(8);
    scenery.getTerrain()->propHeightNoise()->setConfig(0.0);
    scenery.getClouds()->clear();
    scenery.getCamera()->setLocation(VECTOR_ZERO);

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(600, 600);
    renderer.setQuality(0.1);
    renderer.getGodRaysSampler()->setEnabled(false);
    /*SkyRasterizer rasterizer(&renderer, renderer.getProgressHelper(), 0);
    renderer.setSoloRasterizer(&rasterizer);*/

    scenery.getCamera()->setTarget(scenery.getAtmosphere()->childMoon()->getLocation());
    scenery.getCamera()->setFov(0.02);

    scenery.getAtmosphere()->setDayTime(17, 30);
    startTestRender(&renderer, "celestial_bodies_moon_day");

    scenery.getAtmosphere()->setDayTime(23);
    startTestRender(&renderer, "celestial_bodies_moon_night");

    scenery.getAtmosphere()->setDayTime(6);
    scenery.getCamera()->setTarget(scenery.getAtmosphere()->childSun()->getLocation());
    startTestRender(&renderer, "celestial_bodies_sun_horizon");

    scenery.getAtmosphere()->setDayTime(6, 30);
    scenery.getCamera()->setTarget(scenery.getAtmosphere()->childSun()->getLocation());
    startTestRender(&renderer, "celestial_bodies_sun_rising");

    scenery.getAtmosphere()->setDayTime(11);

    scenery.getAtmosphere()->childSun()->propPhi()->setValue(
        scenery.getAtmosphere()->childMoon()->propPhi()->getValue());
    scenery.getAtmosphere()->childSun()->propTheta()->setValue(
        scenery.getAtmosphere()->childMoon()->propTheta()->getValue());
    startTestRender(&renderer, "celestial_bodies_solar_eclipse");
}

static void testCloudsLighting() {
    class FakeModel : public BaseCloudsModel {
      public:
        FakeModel(CloudLayerDefinition *layer, double scale) : BaseCloudsModel(layer), scale(scale) {
        }
        virtual void getAltitudeRange(double *min_altitude, double *max_altitude) const override {
            *min_altitude = 10.0 - scale;
            *max_altitude = 10.0 + scale;
        }
        virtual double getDensity(const Vector3 &location, double) const override {
            return 1.0 - location.sub(Vector3(0.0, 10.0, 0.0)).getNorm() / scale;
        }
        double scale;
    };

    class FakeRasterizer : public OverlayRasterizer {
      public:
        FakeRasterizer(SoftwareCanvasRenderer *renderer, double scale) : OverlayRasterizer(renderer), scale(scale) {
        }
        virtual Color processPixel(int, int, double relx, double rely) const override {
            auto cloud_renderer = renderer->getCloudsRenderer();
            auto atmo_renderer = renderer->getAtmosphereRenderer();
            return cloud_renderer->getColor(Vector3(relx * scale * 1.2, 10.0 + rely * scale * 1.2, scale),
                                            Vector3(relx * scale * 1.2, 10.0 + rely * scale * 1.2, -scale),
                                            atmo_renderer->getSkyColor(Vector3(relx, rely, 1.0).normalize()).final);
        }
        virtual int prepareRasterization() override {
            auto model = new FakeModel(renderer->getScenery()->getClouds()->getCloudLayer(0), scale);
            renderer->getCloudsRenderer()->setLayerModel(0, model);
            renderer->getCloudsRenderer()->setQuality(0.9);
            renderer->getLightingManager()->clearFilters();
            renderer->getLightingManager()->registerFilter(renderer->getCloudsRenderer());
            return OverlayRasterizer::prepareRasterization();
        }
        double scale;
        FakeModel *model;
    };

    Scenery scenery;
    scenery.autoPreset(1);
    scenery.getTerrain()->propHeightNoise()->setConfig(0.0);
    scenery.getCamera()->setTarget(VECTOR_ZERO);
    scenery.getCamera()->setLocation(Vector3(0.0, 10.0, 11.0));

    CloudLayerDefinition layer(NULL, "test");
    scenery.getClouds()->clear();
    scenery.getClouds()->addLayer(layer);

    SoftwareCanvasRenderer renderer(&scenery);
    FakeRasterizer rasterizer(&renderer, 5.0);
    renderer.setSize(800, 800);
    renderer.setSoloRasterizer(&rasterizer);
    renderer.getGodRaysSampler()->setEnabled(false);
    renderer.setAerialPerspectiveEnabled(false);

    scenery.getAtmosphere()->setDayTime(10);
    startTestRender(&renderer, "clouds_lighting_day");
    scenery.getAtmosphere()->setDayTime(6, 30);
    startTestRender(&renderer, "clouds_lighting_dusk");
    scenery.getAtmosphere()->setDayTime(3);
    startTestRender(&renderer, "clouds_lighting_night");
}

static void testCloudModels() {
    Scenery scenery;
    scenery.autoPreset(1);
    scenery.getTerrain()->propHeightNoise()->setConfig(0.0);
    scenery.getCamera()->setLocation(Vector3(0.0, 1.0, 0.0));
    scenery.getCamera()->setTarget(Vector3(0.0, 2.0, -1.0));
    scenery.getCamera()->setFov(Maths::PI_2);

    NoiseState state;
    scenery.getClouds()->getCloudLayer(0)->getNoiseState().copy(&state);

    scenery.getClouds()->clear();
    scenery.getClouds()->addLayer("test");
    auto layer = scenery.getClouds()->getCloudLayer(0);
    state.copy(&layer->noise_state);

    SoftwareCanvasRenderer renderer(&scenery);
    renderer.setSize(800, 600);
    renderer.getGodRaysSampler()->setEnabled(false);
    renderer.setQuality(0.5);

    for (int i = 0; i < CloudLayerDefinition::_COUNT; i++) {
        layer->type = static_cast<CloudLayerDefinition::CloudsType>(i);
        layer->validate();
        renderer.getCloudsRenderer()->update();

        layer->coverage = 0.2;
        startTestRender(&renderer, "clouds_model", i * 4);
        layer->coverage = 0.5;
        startTestRender(&renderer, "clouds_model", i * 4 + 1);
        layer->coverage = 0.8;
        startTestRender(&renderer, "clouds_model", i * 4 + 2);
        layer->coverage = 1.0;
        startTestRender(&renderer, "clouds_model", i * 4 + 3);
    }
}

static void testCanvasAliasing() {
    class FakeRasterizer : public OverlayRasterizer {
      public:
        FakeRasterizer(SoftwareCanvasRenderer *renderer) : OverlayRasterizer(renderer) {
        }
        virtual Color processPixel(int x, int y, double relx, double rely) const override {
            double d = sqrt(relx * relx + rely * rely);
            double s = relx * 0.5 + rely;
            return (x == y or x / 2 == y or y / 2 == x or (d > 0.8 and d < 0.9) or fabs(s) < 0.1)
                       ? COLOR_BLACK
                       : COLOR_WHITE.scaled(5.0);
        }
    };

    Scenery scenery;
    SoftwareCanvasRenderer renderer(&scenery);
    FakeRasterizer rasterizer(&renderer);
    renderer.setSoloRasterizer(&rasterizer);

    for (int i = 0; i < 6; i++) {
        renderer.setSize(400, 400, i + 1);
        startTestRender(&renderer, "canvas_aliasing", i);
    }
}

void runTestSuite() {
    testNoise();
    testCanvasAliasing();
    testTextures();
    testGodRays();
    testCelestialBodies();
    testNearFrustum();
    testCloudsLighting();
    testCloudModels();
    testCloudsNearGround();
    testVegetationModels();
    testOpenGLVegetationImpostor();
    testRasterizationQuality();
    testGroundShadowQuality();
    testCloudQuality();
    testAtmosphereBruneton();
}
