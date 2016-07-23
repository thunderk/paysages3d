#include "CelestialBodyDefinition.h"
#include "BaseTestCase.h"

#include "FloatNode.h"

TEST(CelestialBodyDefinition, getAngularRadius) {
    CelestialBodyDefinition moon(NULL, "moon");
    moon.propDistance()->setValue(384403.0);
    moon.propRadius()->setValue(1737.4);

    EXPECT_DOUBLE_EQ(0.0090394100441593961, moon.getAngularRadius());
}
