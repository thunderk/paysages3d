#include "LightingManager.h"
#include "BaseTestCase.h"

#include "LightFilter.h"
#include "LightSource.h"

class FakeLightSource : public LightSource {
    virtual bool getLightsAt(vector<LightComponent> &, const Vector3 &) const override {
        return false;
    }
};

TEST(LightingManager, registerSource) {
    LightingManager manager;
    FakeLightSource source;

    EXPECT_EQ(0, manager.getSourcesCount());

    manager.registerSource(&source);

    EXPECT_EQ(1, manager.getSourcesCount());

    manager.registerSource(&source);

    EXPECT_EQ(1, manager.getSourcesCount());

    manager.unregisterSource(&source);

    EXPECT_EQ(0, manager.getSourcesCount());
}

class FakeLightFilter : public LightFilter {
    virtual bool applyLightFilter(LightComponent &, const Vector3 &) override {
        return false;
    }
};

TEST(LightingManager, registerFilter) {
    LightingManager manager;
    FakeLightFilter filter;

    EXPECT_EQ(0, manager.getFiltersCount());

    manager.registerFilter(&filter);

    EXPECT_EQ(1, manager.getFiltersCount());

    manager.registerFilter(&filter);

    EXPECT_EQ(1, manager.getFiltersCount());

    manager.unregisterFilter(&filter);

    EXPECT_EQ(0, manager.getFiltersCount());
}
