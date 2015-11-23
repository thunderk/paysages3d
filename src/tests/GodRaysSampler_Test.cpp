#include "BaseTestCase.h"
#include "GodRaysSampler.h"

#include "SpaceSegment.h"
#include "Vector3.h"
#include "LightingManager.h"
#include "LightSource.h"
#include "LightFilter.h"
#include "LightComponent.h"
#include "Color.h"

TEST(GodRaysSampler, getBounds) {
    GodRaysSampler sampler;

    sampler.setAltitudes(-5.0, 300.0);
    sampler.setCameraLocation(Vector3(100.0, 0.0, 8000.0));
    sampler.setQuality(10.0, 500.0, 1.0);
    sampler.reset();

    SpaceSegment bounds = sampler.getBounds();

    EXPECT_VECTOR3_COORDS(bounds.getStart(), -400.0, -5.0, 7500.0);
    EXPECT_VECTOR3_COORDS(bounds.getEnd(), 600.0, 300.0, 8500.0);
}

TEST(GodRaysSampler, getSamples) {
    GodRaysSampler sampler;

    sampler.setAltitudes(-50.0, 100.0);
    sampler.setQuality(10.0, 100.0, 1.0);
    sampler.reset();

    int x, y, z;
    sampler.getSamples(&x, &y, &z);

    EXPECT_EQ(21, x);
    EXPECT_EQ(16, y);
    EXPECT_EQ(21, z);
}

TEST(GodRaysSampler, setQuality) {
    GodRaysSampler sampler;

    sampler.setQuality(0.0);
    EXPECT_DOUBLE_EQ(5.0, sampler.getSamplingStep());
    EXPECT_DOUBLE_EQ(100.0, sampler.getMaxLength());
    EXPECT_DOUBLE_EQ(5.0, sampler.getWalkStep());

    sampler.setQuality(0.5);
    EXPECT_DOUBLE_EQ(1.0, sampler.getSamplingStep());
    EXPECT_DOUBLE_EQ(100.0, sampler.getMaxLength());
    EXPECT_DOUBLE_EQ(0.12195121951, sampler.getWalkStep());

    sampler.setQuality(1.0);
    EXPECT_DOUBLE_EQ(0.55555555555, sampler.getSamplingStep());
    EXPECT_DOUBLE_EQ(100.0, sampler.getMaxLength());
    EXPECT_DOUBLE_EQ(0.06172839506, sampler.getWalkStep());
}

class GodRayLightSource : public LightSource {
    virtual bool getLightsAt(std::vector<LightComponent> &result, const Vector3 &location) const override {
        LightComponent light;
        light.direction = VECTOR_DOWN;
        light.altered = true;
        light.color = Color(fabs(location.x), fabs(location.y), fabs(location.z));
        result.push_back(light);
        return true;
    }
};

TEST(GodRaysSampler, getRawLight) {
    class TestLightFilter : public LightFilter {
        virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override {
            if (at.x > 0.5) {
                light.color.r = 0.1;
                return true;
            } else {
                return true;
            }
        }
    };

    GodRayLightSource light_source;
    TestLightFilter filter;
    LightingManager lighting;
    lighting.registerSource(&light_source);
    lighting.registerFilter(&filter);

    GodRaysSampler sampler;
    sampler.setLighting(&lighting);

    EXPECT_COLOR_RGBA(sampler.getRawLight(Vector3(0.0, 0.0, 0.0), false), 0.0, 0.0, 0.0, 1.0);
    EXPECT_COLOR_RGBA(sampler.getRawLight(Vector3(1.0, 2.0, -6.0), false), 1.0, 2.0, 6.0, 1.0);

    EXPECT_COLOR_RGBA(sampler.getRawLight(Vector3(0.0, 0.0, 0.0), true), 0.0, 0.0, 0.0, 1.0);
    EXPECT_COLOR_RGBA(sampler.getRawLight(Vector3(1.0, 2.0, -6.0), true), 0.1, 2.0, 6.0, 1.0);
}

TEST(GodRaysSampler, getCachedLight) {
    class TestLightFilter : public LightFilter {
        virtual bool applyLightFilter(LightComponent &, const Vector3 &at) override {
            return at.x <= 10.0 or at.x >= 50.0;
        }
    };

    GodRayLightSource light_source;
    TestLightFilter filter;
    LightingManager lighting;
    lighting.registerSource(&light_source);
    lighting.registerFilter(&filter);

    GodRaysSampler sampler;

    sampler.setLighting(&lighting);
    sampler.setQuality(5.0, 100.0, 1.0);
    sampler.setAltitudes(-10.0, 10.0);
    sampler.reset();

    // outside the sampler, considered full light
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(0.0, 15.0, 0.0)));
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(0.0, -15.0, 0.0)));
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(-120.0, 0.0, 0.0)));
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(120.0, 0.0, 0.0)));
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(0.0, 0.0, -100.1)));
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(0.0, 0.0, 100.1)));

    // at sampling point
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(0.0, 0.0, 0.0)));
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(0.0, 5.0, 10.0)));
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(5.0, 5.0, 10.0)));
    EXPECT_DOUBLE_EQ(1.0, sampler.getCachedLight(Vector3(10.0, 5.0, 10.0)));
    EXPECT_DOUBLE_EQ(0.0, sampler.getCachedLight(Vector3(15.0, 5.0, 10.0)));
    EXPECT_DOUBLE_EQ(0.0, sampler.getCachedLight(Vector3(20.0, 5.0, 10.0)));

    // in between sampling points
    EXPECT_DOUBLE_EQ(0.5, sampler.getCachedLight(Vector3(12.5, 5.0, 10.0)));
}
